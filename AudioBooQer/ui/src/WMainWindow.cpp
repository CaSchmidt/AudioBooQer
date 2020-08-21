/****************************************************************************
** Copyright (c) 2014, Carsten Schmidt. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#include <QtConcurrent/QtConcurrentMap>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QThreadPool>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include <csQt/csQtUtil.h>
#include <csUtil/csWProgressLogger.h>

#include "WMainWindow.h"
#include "ui_WMainWindow.h"

#include "BinderIO.h"
#include "Chapter.h"
#include "ChapterModel.h"
#include "Mpeg4Audio.h"
#include "Output.h"
#include "WBookBinder.h"
#include "WTagEditor.h"

////// Private ///////////////////////////////////////////////////////////////

namespace priv {

  void complementJobs(Jobs& jobs, const csILogger *logger, const QString& outputDirPath,
                      const Ui::WMainWindow *ui)
  {
    for(Job& job : jobs) {
      job.format        = ui->formatWidget->format();
      job.logger        = logger;
      job.outputDirPath = outputDirPath;
      job.renameInput   = ui->renameCheck->isChecked();
    }
  }

  BookBinder makeBinder(JobResults results)
  {
    std::sort(results.begin(), results.end());

    BookBinder binder;
    for(const JobResult& result : results) {
      binder.emplace_back(result.title.toStdU16String(), result.outputFilePath.toStdString());
    }
    return binder;
  }

} // namespace priv

////// public ////////////////////////////////////////////////////////////////

WMainWindow::WMainWindow(QWidget *parent, Qt::WindowFlags flags)
  : QMainWindow(parent, flags)
  , ui(new Ui::WMainWindow)
{
  ui->setupUi(this);

  // Chapters Model //////////////////////////////////////////////////////////

  ChapterModel *model = new ChapterModel(ui->chaptersView);
  ui->chaptersView->setModel(model);

  ui->chapterNoCheck->setChecked(model->showChapterNo());
  ui->firstChapterSpin->setValue(model->firstChaopterNo());
  ui->numberWidthSpin->setValue(model->widthChapterNo());

  ui->dissolveChapterAction->setShortcut(Qt::CTRL+Qt::Key_D);
  connect(ui->dissolveChapterAction, SIGNAL(triggered()),
          model, SLOT(dissolveLastChapter()));

  connect(ui->chaptersView, &QTreeView::activated,
          model, &ChapterModel::activateNode);

  connect(model, &ChapterModel::playedFile,
          ui->playerWidget, &WAudioPlayer::playFile);
  connect(ui->playerWidget, &WAudioPlayer::fileNameChanged,
          model, &ChapterModel::setPlayingFileName);

  connect(ui->chapterNoCheck, SIGNAL(toggled(bool)),
          model, SLOT(setShowChapterNo(bool)));
  connect(ui->firstChapterSpin, SIGNAL(valueChanged(int)),
          model, SLOT(setFirstChapterNo(int)));
  connect(ui->numberWidthSpin, SIGNAL(valueChanged(int)),
          model, SLOT(setWidthChapterNo(int)));

  // Buttons /////////////////////////////////////////////////////////////////

  connect(ui->goButton, SIGNAL(clicked()), SLOT(processJobs()));

  // File Menu ///////////////////////////////////////////////////////////////

  ui->openDirAction->setShortcut(QKeySequence::Open);
  connect(ui->openDirAction, SIGNAL(triggered()), SLOT(openDirectory()));

  connect(ui->bindBookAction, &QAction::triggered, this, &WMainWindow::bindBook);

  connect(ui->editTagAction, &QAction::triggered, this, &WMainWindow::editTag);

  ui->quitAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Q));
  connect(ui->quitAction, SIGNAL(triggered()), SLOT(close()));

  // Edit Menu ///////////////////////////////////////////////////////////////

  ui->newChapterAction->setShortcut(QKeySequence::New);
  connect(ui->newChapterAction, SIGNAL(triggered()), SLOT(createNewChapter()));

  // Playback Menu ///////////////////////////////////////////////////////////

  ui->previousAction->setShortcut(Qt::CTRL+Qt::Key_1);
  connect(ui->previousAction, SIGNAL(triggered()),
          ui->playerWidget, SLOT(previous()));

  ui->stopAction->setShortcut(Qt::CTRL+Qt::Key_2);
  connect(ui->stopAction, SIGNAL(triggered()),
          ui->playerWidget, SLOT(stop()));

  ui->playAction->setShortcut(Qt::CTRL+Qt::Key_3);
  connect(ui->playAction, SIGNAL(triggered()),
          ui->playerWidget, SLOT(play()));

  ui->nextAction->setShortcut(Qt::CTRL+Qt::Key_4);
  connect(ui->nextAction, SIGNAL(triggered()),
          ui->playerWidget, SLOT(next()));

  // Languages ///////////////////////////////////////////////////////////////

  ui->languageCombo->addItem(tr("Undetermined"), QString());
  ui->languageCombo->addItem(tr("English"), QStringLiteral("eng"));
  ui->languageCombo->addItem(tr("German"), QStringLiteral("deu"));

  // Threads /////////////////////////////////////////////////////////////////

  ui->threadSpin->setRange(1, qMax<int>(1, QThread::idealThreadCount()));
  ui->threadSpin->setValue(qBound<int>(1, 2, ui->threadSpin->maximum()));
}

WMainWindow::~WMainWindow()
{
  delete ui;
}

////// private slots /////////////////////////////////////////////////////////

void WMainWindow::bindBook()
{
  WBookBinder bookBinder(this);
  if( bookBinder.exec() == QDialog::Rejected ) {
    return;
  }

  const BookBinder binder = bookBinder.binder();
  if( binder.empty() ) {
    return;
  }

  const AacFormat format = ui->formatWidget->format();
  const uint16_t     asc =
      mpeg4::createAudioSpecificConfig(static_cast<uint16_t>(mpeg4::AudioObjectType::AAC_LC),
                                       static_cast<uint16_t>(format.numChannels),
                                       format.numSamplesPerSecond);

  const QString filename =
      QFileDialog::getSaveFileName(this, tr("Save"),
                                   QDir::currentPath(), tr("Audiobooks (*.m4b)"));
  if( filename.isEmpty() ) {
    return;
  }

  outputAdtsBinder(filename.toStdString(), binder, asc, format.numSamplesPerAacFrame,
                   ui->languageCombo->currentData().toString().toStdString());

  QMessageBox::information(this, tr("Info"), tr("Done!"),
                           QMessageBox::Ok, QMessageBox::Ok);
}

void WMainWindow::createNewChapter()
{
  QItemSelectionModel *selection = ui->chaptersView->selectionModel();
  const QModelIndexList indexes = selection->selectedIndexes();
  if( indexes.size() != 1 ) {
    return;
  }
  ChapterModel *model = dynamic_cast<ChapterModel*>(ui->chaptersView->model());
  if( model != nullptr ) {
    const QModelIndex index = model->createNewChapter(indexes.front());
    if( index.isValid() ) {
      ui->chaptersView->expand(index);
    }
  }
}

void WMainWindow::editTag()
{
  const QString filename =
      QFileDialog::getOpenFileName(this, tr("Open"),
                                   QDir::currentPath(), tr("MP4 files (*.mp4 *.m4a *.m4b *.m4v)"));
  if( filename.isEmpty() ) {
    return;
  }

  const Mp4Tag in = Mp4Tag::read(filename.toStdString());
  if( !in.isValid() ) {
    return;
  }

  WTagEditor editor(in, this);
  if( editor.exec() == QDialog::Accepted ) {
    const Mp4Tag out = editor.get();
    if( !out.write() ) {
      QMessageBox::critical(this, tr("Error"),
                            tr("Error writing tag (Mp4Tag::write(\"%1\"))!").arg(out.filename));
    }
  }

  QDir::setCurrent(QFileInfo(filename).absolutePath());

  QMessageBox::information(this, tr("Info"), tr("Done!"),
                           QMessageBox::Ok, QMessageBox::Ok);
}

void WMainWindow::openDirectory()
{
  const QString dirPath =
      QFileDialog::getExistingDirectory(this, tr("Open directory"),
                                        QDir::currentPath());
  if( dirPath.isEmpty() ) {
    return;
  }

  ui->playerWidget->reset();

  const QDir dir(dirPath);
  const QList<QFileInfo> audioFiles =
      dir.entryInfoList({QStringLiteral("*.mp3"), QStringLiteral("*.wav")},
                        QDir::Files, QDir::Name);

  QStringList fileNames;
  foreach(const QFileInfo& fileInfo, audioFiles) {
    fileNames.push_back(fileInfo.absoluteFilePath());
  }

  ChapterRoot *root = new ChapterRoot();
  ChapterNode *sources = new ChapterNode(root, true);
  root->insert(sources);
  sources->setTitle(tr("<Files>"));
  sources->setFiles(fileNames);

  ChapterModel *model = dynamic_cast<ChapterModel*>(ui->chaptersView->model());
  if( model != nullptr ) {
    model->setData(root);
  }

  ui->playerWidget->setFiles(model->files());
  ui->chaptersView->expandAll();
  QDir::setCurrent(dirPath);
}

void WMainWindow::processJobs()
{
  // (1) Sanity check UI /////////////////////////////////////////////////////

  if( !ui->formatWidget->format().isValid() ) {
    return;
  }

  // (2) Query destination folder ////////////////////////////////////////////

  const QString outputDirPath =
      QFileDialog::getExistingDirectory(this,
                                        tr("Open Directory"),
                                        QDir::currentPath());
  if( outputDirPath.isEmpty() ) {
    return;
  }

  // (3) Build jobs from ChapterModel ////////////////////////////////////////

  ChapterModel *model = dynamic_cast<ChapterModel*>(ui->chaptersView->model());
  if( model == nullptr ) {
    return;
  }

  Jobs jobs = model->buildJobs();
  if( jobs.size() < 1 ) {
    return;
  }
  model->deleteJobs();

  // (4) Maintain state of audio player //////////////////////////////////////

  ui->playerWidget->stop();
  ui->playerWidget->setFiles(model->files());

  // (5) Execute jobs ////////////////////////////////////////////////////////

  QThreadPool::globalInstance()->setMaxThreadCount(ui->threadSpin->value());

  csWProgressLogger dialog(this);
  dialog.setWindowTitle(QStringLiteral("Executing jobs..."));

  priv::complementJobs(jobs, dialog.logger(), outputDirPath, ui);

  QFutureWatcher<JobResult> watcher;
  dialog.setFutureWatcher(&watcher);

  QFuture<JobResult> future = QtConcurrent::mapped(jobs, executeJob);
  watcher.setFuture(future);

  dialog.exec();
  future.waitForFinished();

  // (6) (Optionally) Save to binder /////////////////////////////////////////

  if( QMessageBox::question(this, tr("Question"), tr("Create binder?"),
                            QMessageBox::Yes | QMessageBox::No,
                            QMessageBox::No)
      == QMessageBox::Yes ) {
    const QString filename =
        QFileDialog::getSaveFileName(this, tr("Save"), QDir::currentPath(), tr("Binders (*.xml)"));
    if( !filename.isEmpty() ) {
      const BookBinder binder = priv::makeBinder(future.results());
      saveBinder(filename, binder);
    }
  }
}
