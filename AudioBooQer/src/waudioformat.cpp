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

#include <QtCore/QSysInfo>

#include "waudioformat.h"
#include "ui_waudioformat.h"

////// public ////////////////////////////////////////////////////////////////

WAudioFormat::WAudioFormat(QWidget *parent, Qt::WindowFlags f)
  : QWidget(parent, f)
  , ui(new Ui::WAudioFormat)
{
  ui->setupUi(this);

  // Channels ////////////////////////////////////////////////////////////////

  ui->channelsCombo->clear();
  ui->channelsCombo->addItem(QStringLiteral("Mono"), int(1));
  ui->channelsCombo->addItem(QStringLiteral("Stereo"), int(2));
  ui->channelsCombo->setCurrentText(QStringLiteral("Stereo"));

  // Rate ////////////////////////////////////////////////////////////////////

  ui->rateCombo->clear();
  ui->rateCombo->addItem(QStringLiteral("8000Hz"),  int(8000));
  ui->rateCombo->addItem(QStringLiteral("11025Hz"), int(11025));
  ui->rateCombo->addItem(QStringLiteral("22050Hz"), int(22050));
  ui->rateCombo->addItem(QStringLiteral("44100Hz"), int(44100));
  ui->rateCombo->addItem(QStringLiteral("48000Hz"), int(48000));
  ui->rateCombo->setCurrentText(QStringLiteral("44100Hz"));

  // Bits ////////////////////////////////////////////////////////////////////

  ui->bitsCombo->clear();
  ui->bitsCombo->addItem(QStringLiteral("8"),  int(8));
  ui->bitsCombo->addItem(QStringLiteral("16"), int(16));
  ui->bitsCombo->setCurrentText(QStringLiteral("16"));

  // Type ////////////////////////////////////////////////////////////////////

  ui->typeCombo->clear();
  ui->typeCombo->addItem(QStringLiteral("Signed"));
  ui->typeCombo->addItem(QStringLiteral("Unsigned"));
  ui->typeCombo->setCurrentText(QStringLiteral("Signed"));
}

WAudioFormat::~WAudioFormat()
{
  delete ui;
}

QAudioFormat WAudioFormat::format() const
{
  QAudioFormat result;

  result.setByteOrder(QSysInfo::ByteOrder == QSysInfo::LittleEndian
                      ? QAudioFormat::LittleEndian
                      : QAudioFormat::BigEndian);
  result.setChannelCount(ui->channelsCombo->currentData().toInt());
  result.setCodec(QStringLiteral("audio/x-raw"));
  result.setSampleRate(ui->rateCombo->currentData().toInt());
  result.setSampleSize(ui->bitsCombo->currentData().toInt());
  result.setSampleType(ui->typeCombo->currentText() == QStringLiteral("Unsigned")
                       ? QAudioFormat::UnSignedInt
                       : QAudioFormat::SignedInt);

  return result;
}
