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

#include "WAudioFormat.h"
#include "ui_WAudioFormat.h"

////// public ////////////////////////////////////////////////////////////////

WAudioFormat::WAudioFormat(QWidget *parent, Qt::WindowFlags f)
  : QWidget(parent, f)
  , ui(new Ui::WAudioFormat)
{
  ui->setupUi(this);

  // Channels ////////////////////////////////////////////////////////////////

  ui->channelsCombo->clear();
  ui->channelsCombo->addItem(QStringLiteral("Mono"),   static_cast<unsigned int>(1));
  ui->channelsCombo->addItem(QStringLiteral("Stereo"), static_cast<unsigned int>(2));
  ui->channelsCombo->setCurrentText(QStringLiteral("Stereo"));

  // Rate ////////////////////////////////////////////////////////////////////

  ui->rateCombo->clear();
  for(unsigned int i = 0; i < AacFormat::numSupportedRates(); i++) {
    const unsigned int rate = AacFormat::supportedRates[i];
    ui->rateCombo->addItem(QString::number(rate) + QStringLiteral("Hz"), rate);
  }
  ui->rateCombo->setCurrentText(QStringLiteral("22050Hz"));

  // Bits ////////////////////////////////////////////////////////////////////

  ui->bitsCombo->clear();
  ui->bitsCombo->addItem(QStringLiteral("8"),  static_cast<unsigned int>(8));
  ui->bitsCombo->addItem(QStringLiteral("16"), static_cast<unsigned int>(16));
  ui->bitsCombo->setCurrentText(QStringLiteral("16"));
}

WAudioFormat::~WAudioFormat()
{
  delete ui;
}

AacFormat WAudioFormat::format() const
{
  AacFormat result;
  result.numBitsPerChannel     = ui->bitsCombo->currentData().toUInt();
  result.numChannels           = ui->channelsCombo->currentData().toUInt();
  result.numSamplesPerSecond   = ui->rateCombo->currentData().toUInt();

  return result;
}
