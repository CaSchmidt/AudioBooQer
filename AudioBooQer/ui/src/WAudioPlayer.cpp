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

#include <QtCore/QUrl>
#include <QtMultimedia/QMediaPlaylist>

#include "WAudioPlayer.h"
#include "ui_WAudioPlayer.h"

////// public ////////////////////////////////////////////////////////////////

WAudioPlayer::WAudioPlayer(QWidget *parent, Qt::WindowFlags f)
  : QWidget(parent, f)
  , ui(new Ui::WAudioPlayer)
  , _player(nullptr)
  , _playlist(nullptr)
{
  ui->setupUi(this);

  // Media Player ////////////////////////////////////////////////////////////

  _player = new QMediaPlayer(this);
  connect(_player, &QMediaPlayer::stateChanged,
          this, &WAudioPlayer::changeButtonState);
  connect(_player, &QMediaPlayer::currentMediaChanged,
          this, &WAudioPlayer::changeMediaInfo);

  // Playlist ////////////////////////////////////////////////////////////////

  _playlist = new QMediaPlaylist(this);
  _playlist->setPlaybackMode(QMediaPlaylist::Sequential);
  _player->setPlaylist(_playlist);

  // Buttons /////////////////////////////////////////////////////////////////

  connect(ui->nextButton, &QPushButton::clicked, this, &WAudioPlayer::next);
  connect(ui->stopButton, &QPushButton::clicked, this, &WAudioPlayer::stop);
  connect(ui->playButton, &QPushButton::clicked, this, &WAudioPlayer::play);
  connect(ui->previousButton, &QPushButton::clicked, this, &WAudioPlayer::previous);
}

WAudioPlayer::~WAudioPlayer()
{
  delete ui;
}

void WAudioPlayer::setFiles(const QStringList& files)
{
  reset();

  foreach(const QString& file, files) {
    _playlist->addMedia(QUrl::fromLocalFile(file));
  }
}

////// public slots //////////////////////////////////////////////////////////

void WAudioPlayer::next()
{
  _playlist->next();
}

void WAudioPlayer::play()
{
  if( _player->state() == QMediaPlayer::PlayingState ) {
    _player->pause();
  } else {
    _player->play();
  }
}

void WAudioPlayer::playFile(const QString& filename)
{
  const QMediaContent content(QUrl::fromLocalFile(filename));

  for(int i = 0; i < _playlist->mediaCount(); i++) {
    if( _playlist->media(i) == content ) {
      _playlist->setCurrentIndex(i);
      break;
    }
  }
}

void WAudioPlayer::previous()
{
  _playlist->previous();
}

void WAudioPlayer::stop()
{
  _player->stop();
}

void WAudioPlayer::reset()
{
  _player->stop();
  _playlist->clear();
}

////// private slots /////////////////////////////////////////////////////////

void WAudioPlayer::changeButtonState(QMediaPlayer::State state)
{
  if( state == QMediaPlayer::PlayingState ) {
    ui->playButton->setText(tr("Pause"));
  } else {
    ui->playButton->setText(tr("Play"));
  }

  if( state == QMediaPlayer::StoppedState ) {
    emit fileNameChanged(QString());
  } else {
    changeMediaInfo(_player->currentMedia());
  }
}

void WAudioPlayer::changeMediaInfo(const QMediaContent& content)
{
  if( _player->state() == QMediaPlayer::StoppedState ) {
    emit fileNameChanged(QString());
  } else {
    emit fileNameChanged(content.canonicalUrl().toLocalFile());
  }
}
