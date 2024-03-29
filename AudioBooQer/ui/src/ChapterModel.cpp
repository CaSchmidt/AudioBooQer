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

#include <QtCore/QDir>
#include <QtGui/QColor>

#include <cs/Core/QStringUtil.h>
#include <cs/Qt/AbstractTreeItem.h>

#include "ChapterModel.h"

#include "Chapter.h"

////// Private ///////////////////////////////////////////////////////////////

namespace priv {

  QString autoChapterName(const QStringList& files)
  {
    if( files.isEmpty() ) {
      return QString();
    }
    QString name = QFileInfo(files.front()).completeBaseName();
    name.remove(QRegExp(QStringLiteral("^\\s*"), Qt::CaseInsensitive));
    name.remove(QRegExp(QStringLiteral("^cd\\s*"), Qt::CaseInsensitive));
    name.remove(QRegExp(QStringLiteral("^\\d+\\s*"), Qt::CaseInsensitive));
    name.remove(QRegExp(QStringLiteral("^[-_]+\\s*"), Qt::CaseInsensitive));
    return name.trimmed();
  }

} // namespace priv

////// public ////////////////////////////////////////////////////////////////

ChapterModel::ChapterModel(QObject *parent)
  : QAbstractItemModel(parent)
  , _root(nullptr)
  , _autoChapterName(false)
  , _showChapterNo(false)
  , _firstChapterNo(1)
  , _widthChapterNo(2)
  , _playingFileName()
{
  _root = new ChapterRoot();
}

ChapterModel::~ChapterModel()
{
  delete _root;
}

QModelIndex ChapterModel::createNewChapter(const QModelIndex& index)
{
  ChapterFile *file = dynamic_cast<ChapterFile*>(cs::treeItem(index));
  if( file == nullptr ) {
    return QModelIndex();
  }
  ChapterNode *sources = dynamic_cast<ChapterNode*>(file->parentItem());
  if( sources == nullptr  ||  !sources->isSource() ) {
    return QModelIndex();
  }

  const int count = index.row() + 1;

  const QStringList files = sources->files(count);

  const QModelIndex parent = ChapterModel::parent(index);
  beginRemoveRows(parent, 0, count - 1);
  sources->remove(count);
  endRemoveRows();

  ChapterRoot *root = dynamic_cast<ChapterRoot*>(_root);
  beginInsertRows(QModelIndex(), sources->row(), sources->row());
  ChapterNode *newNode = new ChapterNode(root);
  root->insert(newNode);
  if( _autoChapterName ) {
    const QString title = priv::autoChapterName(files);
    if( !title.isEmpty() ) {
      newNode->setTitle(title);
    }
  }
  if( newNode->title().isEmpty() ) {
    newNode->setTitle(tr("New Chapter"));
  }
  newNode->setFiles(files);
  endInsertRows();

  return ChapterModel::index(newNode->row(), 0, QModelIndex());
}

void ChapterModel::setData(cs::AbstractTreeItem *data)
{
  beginResetModel();

  delete _root;

  if( data != nullptr ) {
    _root = data;
  } else {
    _root = new ChapterRoot();
  }

  endResetModel();
}

bool ChapterModel::autoChapterName() const
{
  return _autoChapterName;
}

bool ChapterModel::showChapterNo() const
{
  return _showChapterNo;
}

int ChapterModel::firstChaopterNo() const
{
  return _firstChapterNo;
}

int ChapterModel::widthChapterNo() const
{
  return _widthChapterNo;
}

Jobs ChapterModel::buildJobs() const
{
  Jobs jobs;

  // NOTE: Don't Build Job for <Files>!!! -> rowCount()-1
  for(int cntNode = 0; cntNode < rowCount(QModelIndex()) - 1; cntNode++) {
    const QModelIndex nodeIndex = index(cntNode, 0, QModelIndex());

    Job job;
    job.position = nodeIndex.row() + _firstChapterNo;

    for(int cntFile = 0; cntFile < rowCount(nodeIndex); cntFile++) {
      const QModelIndex fileIndex = index(cntFile, 0, nodeIndex);

      ChapterFile *file = dynamic_cast<ChapterFile*>(cs::treeItem(fileIndex));
      job.inputFiles.push_back(file->fileName());
    } // File

    ChapterNode *node = dynamic_cast<ChapterNode*>(cs::treeItem(nodeIndex));
    job.title = chapterTitle(node);

    job.outputDirPath = QFileInfo(job.inputFiles.front()).absolutePath();

    jobs.push_back(job);
  } // Node

  return jobs;
}

void ChapterModel::deleteJobs()
{
  if( rowCount(QModelIndex()) < 2 ) {
    return;
  }

  const int numJobs = rowCount(QModelIndex())-1;
  beginRemoveRows(QModelIndex(), 0, numJobs-1);
  ChapterRoot *root = dynamic_cast<ChapterRoot*>(_root);
  for(int i = 0; i < numJobs; i++) {
    root->removeAt(0);
  }
  endRemoveRows();
}

QStringList ChapterModel::files(const bool source_only) const
{
  QStringList result;

  for(int i = 0; i < rowCount(QModelIndex()); i++) {
    const QModelIndex chIndex = index(i, 0, QModelIndex());
    const ChapterNode *chapter = dynamic_cast<const ChapterNode*>(cs::treeItem(chIndex));

    if( source_only  &&  !chapter->isSource() ) {
      continue;
    }

    result += chapter->files(chapter->rowCount());
  }

  qSort(result);

  return result;
}

int ChapterModel::columnCount(const QModelIndex& parent) const
{
  if( parent.isValid() ) {
    return cs::treeItem(parent)->columnCount();
  }
  return _root->columnCount();
}

QVariant ChapterModel::data(const QModelIndex& index, int role) const
{
  if( !index.isValid() ) {
    return QVariant();
  }

  cs::AbstractTreeItem *item = cs::treeItem(index);
  if(        role == Qt::DisplayRole ) {
    ChapterNode *node = dynamic_cast<ChapterNode*>(cs::treeItem(index));
    if( node != nullptr  &&  !node->isSource() ) {
      return chapterTitle(node);
    } else {
      return item->data(index.column());
    }
  } else if( role == Qt::EditRole ) {
    if( isNode(item) ) {
      ChapterNode *node = dynamic_cast<ChapterNode*>(item);
      return node->title();
    }
  } else if( role == Qt::DecorationRole ) {
    if( isFile(item) ) {
      ChapterFile *file = dynamic_cast<ChapterFile*>(item);
      if( file->fileName() == _playingFileName ) {
        return QColor(Qt::green);
      }
    }
  }

  return QVariant();
}

Qt::ItemFlags ChapterModel::flags(const QModelIndex& index) const
{
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  Qt::ItemFlags f(Qt::ItemIsEnabled);

  cs::AbstractTreeItem *item = cs::treeItem(index);
  ChapterNode    *nodeItem = dynamic_cast<ChapterNode*>(item);
  if( isFile(item) ) {
    f |= Qt::ItemIsSelectable;
  } else if( nodeItem != nullptr  &&  !nodeItem->isSource() ) {
    f |= Qt::ItemIsEditable;
  }

  return f;
}

QModelIndex ChapterModel::index(int row, int column,
                                const QModelIndex& parent) const
{
  if( !hasIndex(row, column, parent) ) {
    return QModelIndex();
  }

  cs::AbstractTreeItem *parentItem(nullptr);
  if( !parent.isValid() ) {
    parentItem = _root;
  } else {
    parentItem = cs::treeItem(parent);
  }

  cs::AbstractTreeItem *childItem = parentItem->childItem(row);
  if( childItem != nullptr ) {
    return createIndex(row, column, childItem);
  }

  return QModelIndex();
}

QModelIndex ChapterModel::parent(const QModelIndex& index) const
{
  if( !index.isValid() ) {
    return QModelIndex();
  }

  cs::AbstractTreeItem  *childItem = cs::treeItem(index);
  cs::AbstractTreeItem *parentItem = childItem->parentItem();

  if( parentItem == _root ) {
    return QModelIndex();
  }

  return createIndex(parentItem->row(), 0, parentItem);
}

int ChapterModel::rowCount(const QModelIndex& parent) const
{
  if( parent.column() > 0 ) {
    return 0;
  }

  cs::AbstractTreeItem *parentItem(nullptr);
  if( !parent.isValid() ) {
    parentItem = _root;
  } else {
    parentItem = cs::treeItem(parent);
  }

  return parentItem->rowCount();
}

bool ChapterModel::setData(const QModelIndex& index, const QVariant& value,
                           int role)
{
  ChapterNode *node = dynamic_cast<ChapterNode*>(cs::treeItem(index));
  if(     node == nullptr                     ||  node->isSource()
          ||  value.type() != QMetaType::QString  ||  role != Qt::EditRole ) {
    return false;
  }

  node->setTitle(value.toString());
  emit dataChanged(index, index);

  return true;
}

////// public slots //////////////////////////////////////////////////////////

void ChapterModel::activateNode(const QModelIndex& nodeIndex)
{
  const ChapterFile *file = dynamic_cast<const ChapterFile*>(cs::treeItem(nodeIndex));
  if( file != nullptr ) {
    emit playedFile(file->fileName());
  }
}

void ChapterModel::dissolveLastChapter()
{
  if( rowCount(QModelIndex()) < 2 ) {
    return;
  }

  const int dissolveRow = rowCount(QModelIndex())-2;

  beginRemoveRows(QModelIndex(), dissolveRow, dissolveRow);
  ChapterNode *node = dynamic_cast<ChapterNode*>(_root->childItem(dissolveRow));
  const QStringList dissolvedFiles = node->files(node->rowCount());

  ChapterRoot *root = dynamic_cast<ChapterRoot*>(_root);
  root->removeAt(dissolveRow);
  endRemoveRows();

  const QModelIndex sourcesIndex =
      index(rowCount(QModelIndex())-1, 0, QModelIndex());
  beginInsertRows(sourcesIndex, 0, dissolvedFiles.size()-1);
  ChapterNode *sources = dynamic_cast<ChapterNode*>(cs::treeItem(sourcesIndex));
  sources->insertFiles(dissolvedFiles);
  endInsertRows();
}

void ChapterModel::setAutoChapterName(bool state)
{
  _autoChapterName = state;
}

void ChapterModel::setFirstChapterNo(int no)
{
  _firstChapterNo = no;
  updateChapters();
}

void ChapterModel::setShowChapterNo(bool state)
{
  _showChapterNo = state;
  updateChapters();
}

void ChapterModel::setWidthChapterNo(int width)
{
  _widthChapterNo = width;
  updateChapters();
}

void ChapterModel::setPlayingFileName(const QString& fileName)
{
  _playingFileName = fileName;

  const QVector<int> roles = QVector<int>() << Qt::DecorationRole;
  for(int cntNode = 0; cntNode < rowCount(QModelIndex()); cntNode++) {
    QModelIndex nodeIndex = index(cntNode, 0, QModelIndex());

    for(int cntFile = 0; cntFile < rowCount(nodeIndex); cntFile++) {
      QModelIndex fileIndex = index(cntFile, 0, nodeIndex);

      emit dataChanged(fileIndex, fileIndex, roles);
    }
  }
}

////// private ///////////////////////////////////////////////////////////////

QString ChapterModel::chapterTitle(const class ChapterNode *node) const
{
  if( _showChapterNo ) {
    return QStringLiteral("%1 - %2")
        .arg(_firstChapterNo + node->row(), _widthChapterNo, 10, cs::L1C('0'))
        .arg(node->title());
  }
  return node->title();
}

void ChapterModel::updateChapters()
{
  const QVector<int> roles = QVector<int>() << Qt::DisplayRole;
  for(int cntNode = 0; cntNode < rowCount(QModelIndex()); cntNode++) {
    QModelIndex nodeIndex = index(cntNode, 0, QModelIndex());

    emit dataChanged(nodeIndex, nodeIndex, roles);
  }
}
