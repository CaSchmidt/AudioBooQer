/****************************************************************************
** Copyright (c) 2019, Carsten Schmidt. All rights reserved.
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

#include <QtCore/QDir>
#include <QtWidgets/QFileDialog>

#include "WBookBinder.h"
#include "ui_WBookBinder.h"

#include "BookBinderModel.h"

////// public ////////////////////////////////////////////////////////////////

WBookBinder::WBookBinder(QWidget *parent, Qt::WindowFlags f)
  : QDialog(parent, f)
  , ui(new Ui::WBookBinder)
{
  ui->setupUi(this);

  // Data Model //////////////////////////////////////////////////////////////

  _model = new BookBinderModel(this);
  ui->chaptersList->setModel(_model);

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(ui->addButton,    &QPushButton::clicked, this, &WBookBinder::addChapters);
  connect(ui->upButton,     &QPushButton::clicked, this, &WBookBinder::moveChapterUp);
  connect(ui->downButton,   &QPushButton::clicked, this, &WBookBinder::moveChapterDown);
  connect(ui->removeButton, &QPushButton::clicked, this, &WBookBinder::removeChapter);
}

WBookBinder::~WBookBinder()
{
  delete ui;
}

////// private slots /////////////////////////////////////////////////////////

void WBookBinder::addChapters()
{
  QStringList files =
      QFileDialog::getOpenFileNames(this, tr("Select chapters"),
                                    QDir::currentPath(), tr("Chapters (*.aac)"));
  if( files.isEmpty() ) {
    return;
  }

  qSort(files);

  BookBinder binder;
  for(const QString& file : files) {
    const QFileInfo info(file);
    const BookBinderChapter chapter{info.completeBaseName(), info.absoluteFilePath()};
    binder.push_back(chapter);
  }

  _model->appendChapters(binder);

  QDir::setCurrent(QFileInfo(binder.last().second).absolutePath());
}

void WBookBinder::moveChapterDown()
{
  moveChapter(MOVE_DOWN);
}

void WBookBinder::moveChapterUp()
{
  moveChapter(MOVE_UP);
}

void WBookBinder::removeChapter()
{
  const QModelIndex index = singleSelection();
  if( !index.isValid() ) {
    return;
  }
  _model->removeChapter(index.row());
}

////// private ///////////////////////////////////////////////////////////////

void WBookBinder::moveChapter(const bool is_up)
{
  const QModelIndex index = singleSelection();
  if( !index.isValid() ) {
    return;
  }
  const QModelIndex moved = _model->move(index.row(), is_up);
  if( moved.isValid() ) {
    ui->chaptersList->setCurrentIndex(moved);
  }
  ui->chaptersList->setFocus();
}

QModelIndex WBookBinder::singleSelection() const
{
  const QModelIndexList indexes = ui->chaptersList->selectionModel()->selection().indexes();
  if( indexes.size() != 1 ) {
    return QModelIndex();
  }
  return indexes.front();
}
