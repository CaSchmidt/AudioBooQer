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

#ifndef WBOOKBINDER_H
#define WBOOKBINDER_H

#include <QtWidgets/QDialog>

#include "BookBinderModel.h"

namespace Ui {
  class WBookBinder;
}

class WBookBinder : public QDialog {
  Q_OBJECT
public:
  WBookBinder(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
  ~WBookBinder();

  BookBinder binder() const;

private slots:
  void addChapters();
  void moveChapterDown();
  void moveChapterUp();
  void removeChapter();

protected:
  bool eventFilter(QObject *watched, QEvent *event);

private:
  static constexpr bool MOVE_DOWN = false;
  static constexpr bool MOVE_UP   = true;

  void moveChapter(const bool is_up);
  void openBinder();
  void saveBinder();
  QModelIndex singleSelection() const;

  Ui::WBookBinder *ui{nullptr};
  BookBinderModel *_model{nullptr};
};

#endif // WBOOKBINDER_H
