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

#include <algorithm>

#include <QtCore/QFileInfo>

#include "BookBinderModel.h"

////// public ////////////////////////////////////////////////////////////////

BookBinderModel::BookBinderModel(QObject *parent)
  : QAbstractListModel(parent)
{
}

BookBinderModel::~BookBinderModel()
{
}

QVariant BookBinderModel::data(const QModelIndex& index, int role) const
{
  if( !index.isValid() ) {
    return QVariant();
  }
  if(        role == Qt::DisplayRole ) {
    return _binder.at(index.row()).first;
  } else if( role == Qt::EditRole ) {
    return _binder.at(index.row()).first;
  } else if( role == Qt::ToolTipRole ) {
    return _binder.at(index.row()).second;
  }
  return QVariant();
}

Qt::ItemFlags BookBinderModel::flags(const QModelIndex& index) const
{
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }
  Qt::ItemFlags f = QAbstractListModel::flags(index);
  f |= Qt::ItemIsEditable;
  f |= Qt::ItemIsSelectable;
  return f;
}

int BookBinderModel::rowCount(const QModelIndex& /*index*/) const
{
  return _binder.size();
}

bool BookBinderModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if( !index.isValid()                  ||  !isChapter(index.row())  ||
      value.type() != QVariant::String  ||  role != Qt::EditRole ) {
    return false;
  }
  _binder[index.row()].first = value.toString();
  emit dataChanged(index, index);
  return true;
}

void BookBinderModel::appendChapters(const BookBinder& chapters)
{
  if( chapters.isEmpty() ) {
    return;
  }
  beginInsertRows(QModelIndex(), _binder.size(), _binder.size() + chapters.size() - 1);
  _binder.append(chapters);
  endInsertRows();
}

void BookBinderModel::apply(const StringOperation op)
{
  beginResetModel();
  for(BookBinderChapter& chapter : _binder) {
    if( op == AllStringOperations  ||  op == RemoveNumbering ) {
      chapter.first.remove(QRegExp(QStringLiteral("^\\d+[_ ]")));
    }
    if( op == AllStringOperations  ||  op == ReplaceUnderscore ) {
      chapter.first.replace(QChar::fromLatin1('_'), QChar::fromLatin1(' '));
    }
    if( op == AllStringOperations  ||  op == Simplify ) {
      chapter.first = chapter.first.simplified();
    }
    if( op == AllStringOperations  ||  op == Trim ) {
      chapter.first = chapter.first.trimmed();
    }
  }
  endResetModel();
}

BookBinder BookBinderModel::binder() const
{
  return _binder;
}

bool BookBinderModel::isChapter(const int i) const
{
  return 0 <= i  &&  i < _binder.size();
}

QModelIndex BookBinderModel::move(const int from, const bool is_up)
{
  const int to = is_up
      ? from - 1
      : from + 1;
  if( !isChapter(from)  ||  !isChapter(to) ) {
    return QModelIndex();
  }
  _binder.swap(from, to);
  emit dataChanged(index(from, 0, QModelIndex()),
                   index(to,   0, QModelIndex()));
  return index(to, 0, QModelIndex());
}

void BookBinderModel::removeChapter(const int i)
{
  if( !isChapter(i) ) {
    return;
  }
  beginRemoveRows(QModelIndex(), i, i);
  _binder.removeAt(i);
  endRemoveRows();
}

void BookBinderModel::resetChapters()
{
  beginResetModel();
  for(BookBinderChapter& chapter : _binder) {
    chapter.first = QFileInfo(chapter.second).completeBaseName();
  }
  endResetModel();
}

void BookBinderModel::setBinder(const BookBinder& binder)
{
  beginResetModel();
  _binder = binder;
  endResetModel();
}

void BookBinderModel::sortByChapter()
{
  beginResetModel();
  std::sort(_binder.begin(), _binder.end(),
            [](const BookBinderChapter& a, const BookBinderChapter& b) -> bool {
    return a.first < b.first;
  });
  endResetModel();
}

void BookBinderModel::sortByFilename()
{
  beginResetModel();
  std::sort(_binder.begin(), _binder.end(),
            [](const BookBinderChapter& a, const BookBinderChapter& b) -> bool {
    return a.second < b.second;
  });
  endResetModel();
}
