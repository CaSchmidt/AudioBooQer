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

#include <limits>

#include <QtWidgets/QPushButton>

#include <csQt/csDialogButtonBox.h>

#include "WTagEditor.h"
#include "ui_WTagEditor.h"

////// public ////////////////////////////////////////////////////////////////

WTagEditor::WTagEditor(const Mp4Tag& tag, QWidget *parent, Qt::WindowFlags f)
  : QDialog(parent, f)
  , ui(new Ui::WTagEditor)
{
  ui->setupUi(this);

  // QSpinBox ////////////////////////////////////////////////////////////////

  constexpr uint16_t mx = std::numeric_limits<uint16_t>::max();
  ui->trackIndexSpin->setRange(1, mx);
  ui->trackTotalSpin->setRange(1, mx);
  ui->diskIndexSpin->setRange(1, mx);
  ui->diskTotalSpin->setRange(1, mx);

  ui->trackIndexSpin->setValue(1);
  ui->trackTotalSpin->setValue(1);
  ui->diskIndexSpin->setValue(1);
  ui->diskTotalSpin->setValue(1);

  // Buttons /////////////////////////////////////////////////////////////////

  csRemoveAllButtons(ui->buttonBox);
  csAddButton(ui->buttonBox, tr("Save"),    QDialogButtonBox::AcceptRole, false, true);
  csAddButton(ui->buttonBox, tr("Discard"), QDialogButtonBox::RejectRole);

  // Initialize //////////////////////////////////////////////////////////////

  set(tag);
}

WTagEditor::~WTagEditor()
{
  delete ui;
}

Mp4Tag WTagEditor::get() const
{
  Mp4Tag result;

  result.filename    = _filename;
  result.title       = ui->titleEdit->text();
  result.chapter     = ui->chapterEdit->text();
  result.author      = ui->authorEdit->text();
  result.albumArtist = ui->albumArtistEdit->text();
  result.composer    = ui->composerEdit->text();
  result.genre       = ui->genreEdit->text();
  result.trackIndex  = static_cast<uint16_t>(ui->trackIndexSpin->value());
  result.trackTotal  = static_cast<uint16_t>(ui->trackTotalSpin->value());
  result.diskIndex   = static_cast<uint16_t>(ui->diskIndexSpin->value());
  result.diskTotal   = static_cast<uint16_t>(ui->diskTotalSpin->value());

  return result;
}

void WTagEditor::set(const Mp4Tag& tag)
{
  _filename = tag.filename;

  ui->titleEdit->setText(tag.title);
  ui->chapterEdit->setText(tag.chapter);
  ui->authorEdit->setText(tag.author);
  ui->albumArtistEdit->setText(tag.albumArtist);
  ui->composerEdit->setText(tag.composer);
  ui->genreEdit->setText(tag.genre);
  ui->trackIndexSpin->setValue(tag.trackIndex);
  ui->trackTotalSpin->setValue(tag.trackTotal);
  ui->diskIndexSpin->setValue(tag.diskIndex);
  ui->diskTotalSpin->setValue(tag.diskTotal);
}
