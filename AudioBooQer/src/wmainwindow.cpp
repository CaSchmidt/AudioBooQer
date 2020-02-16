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

#include <QtWidgets/QApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include <csQt/csQtUtil.h>

#include "wmainwindow.h"
#include "ui_wmainwindow.h"

#include "chapter.h"
#include "chaptermodel.h"
#include "Mpeg4Audio.h"
#include "Output.h"
#include "WBookBinder.h"
#include "wjobinfo.h"
#include "WTagEditor.h"

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

  // Load Settings ///////////////////////////////////////////////////////////

  loadSettings();
}

WMainWindow::~WMainWindow()
{
  saveSettings();

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

  outputAdtsBinder(filename.toStdString(), binder, asc, format.numSamplesPerAacFrame);
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

////// private ///////////////////////////////////////////////////////////////

void WMainWindow::loadSettings()
{
  QSettings settings(settingsFileName(), QSettings::IniFormat);
}

void WMainWindow::saveSettings() const
{
  QSettings settings(settingsFileName(), QSettings::IniFormat);

  settings.sync();
}

void WMainWindow::processJobs()
{
  if( !ui->formatWidget->format().isValid() ) {
    return;
  }

  const QString outputDirPath =
      QFileDialog::getExistingDirectory(this,
                                        tr("Open Directory"),
                                        QDir::currentPath());
  if( outputDirPath.isEmpty() ) {
    return;
  }

  ChapterModel *model = dynamic_cast<ChapterModel*>(ui->chaptersView->model());
  if( model == nullptr ) {
    return;
  }

  Jobs jobs = model->buildJobs();
  if( jobs.size() < 1 ) {
    return;
  }

  ui->playerWidget->stop();

  for(Job& job : jobs) {
    job.format        = ui->formatWidget->format();
    job.outputDirPath = outputDirPath;
    job.renameInput   = ui->renameCheck->isChecked();
  }

  WJobInfo jobInfo(this);
  jobInfo.executeJobs(jobs);

  model->deleteJobs();
  ui->playerWidget->setFiles(model->files());
}

QString WMainWindow::settingsFileName()
{
  return QFileInfo(QDir(QApplication::applicationDirPath()),
                   _L1("AudioBooQer.ini")).absoluteFilePath();
}
