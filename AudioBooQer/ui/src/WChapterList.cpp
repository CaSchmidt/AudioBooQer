/****************************************************************************
** Copyright (c) 2020, Carsten Schmidt. All rights reserved.
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

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QListView>
#include <QtWidgets/QMenu>

#include <cs/Core/QStringUtil.h>

#include "WChapterList.h"

#include "BinderIO.h"

////// public ////////////////////////////////////////////////////////////////

WChapterList::WChapterList(QWidget *parent, Qt::WindowFlags f)
  : cs::WListEditor(parent, f)
{
  // User Interface //////////////////////////////////////////////////////////

  setShowContextMenu(true);

  view()->setAlternatingRowColors(true);
  view()->setEditTriggers(QAbstractItemView::EditKeyPressed);
  view()->setSelectionBehavior(QAbstractItemView::SelectRows);
  view()->setSelectionMode(QAbstractItemView::SingleSelection);

  // Data Model //////////////////////////////////////////////////////////////

  _model = new BookBinderModel(this);
  view()->setModel(_model);
}

WChapterList::~WChapterList()
{
}

BookBinder WChapterList::binder() const
{
  return _model->binder();
}

////// private ///////////////////////////////////////////////////////////////

void WChapterList::moveChapter(const bool is_up)
{
  const QModelIndex index = singleSelection();
  if( !index.isValid() ) {
    return;
  }
  const QModelIndex moved = _model->move(index.row(), is_up);
  if( moved.isValid() ) {
    view()->setCurrentIndex(moved);
  }
  view()->setFocus();
}

void WChapterList::onAdd()
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
    const BookBinderChapter chapter(cs::toUtf8String(info.completeBaseName()),
                                    cs::toPath(info.absoluteFilePath()));
    binder.push_back(chapter);
  }

  _model->appendChapters(binder);

  QDir::setCurrent(QFileInfo(cs::toQString(binder.back().second)).absolutePath());
}

void WChapterList::onContextMenu(const QPoint& globalPos)
{
  QMenu menu;
  QAction *open     = menu.addAction(tr("Open binder..."));
  QAction *save     = menu.addAction(tr("Save binder..."));
  menu.addSeparator();
  QAction *remove   = menu.addAction(tr("Remove numbering"));
  QAction *replace  = menu.addAction(tr("Replace underscore with space"));
  QAction *simplify = menu.addAction(tr("Simplify"));
  QAction *trim     = menu.addAction(tr("Trim"));
  menu.addSeparator();
  QAction *allOperations = menu.addAction(tr("All operations"));
  menu.addSeparator();
  QAction *sortChapter  = menu.addAction(tr("Sort by chapter"));
  QAction *sortFilename = menu.addAction(tr("Sort by filename"));
  menu.addSeparator();
  QAction *resetChapters = menu.addAction(tr("Reset chapter titles"));

  QAction *choice = menu.exec(globalPos);
  if(        choice == nullptr ) {
    return;
  } else if( choice == open ) {
    openBinder();
  } else if( choice == save ) {
    saveBinder();
  } else if( choice == remove ) {
    _model->apply(BookBinderModel::RemoveNumbering);
  } else if( choice == replace ) {
    _model->apply(BookBinderModel::ReplaceUnderscore);
  } else if( choice == simplify ) {
    _model->apply(BookBinderModel::Simplify);
  } else if( choice == trim ) {
    _model->apply(BookBinderModel::Trim);
  } else if( choice == allOperations ) {
    _model->apply(BookBinderModel::AllStringOperations);
  } else if( choice == sortChapter ) {
    _model->sortByChapter();
  } else if( choice == sortFilename ) {
    _model->sortByFilename();
  } else if( choice == resetChapters ) {
    _model->resetChapters();
  }
}

void WChapterList::onDown()
{
  moveChapter(MOVE_DOWN);
}

void WChapterList::onRemove()
{
  const QModelIndex index = singleSelection();
  if( !index.isValid() ) {
    return;
  }
  _model->removeChapter(index.row());
}

void WChapterList::onUp()
{
  moveChapter(MOVE_UP);
}

void WChapterList::openBinder()
{
  const QString filename =
      QFileDialog::getOpenFileName(this, tr("Open"), QDir::currentPath(), tr("Binders (*.xml)"));
  if( filename.isEmpty() ) {
    return;
  }
  const BookBinder binder = ::openBinder(filename);
  _model->appendChapters(binder);
}

void WChapterList::saveBinder()
{
  const QString filename =
      QFileDialog::getSaveFileName(this, tr("Save"), QDir::currentPath(), tr("Binders (*.xml)"));
  if( filename.isEmpty() ) {
    return;
  }
  const BookBinder binder = _model->binder();
  ::saveBinder(filename, binder);
}

QModelIndex WChapterList::singleSelection() const
{
  const QModelIndexList indexes = view()->selectionModel()->selection().indexes();
  if( indexes.size() != 1 ) {
    return QModelIndex();
  }
  return indexes.front();
}
