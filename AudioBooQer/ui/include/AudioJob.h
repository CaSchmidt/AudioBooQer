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

#ifndef AUDIOJOB_H
#define AUDIOJOB_H

#include <QtMultimedia/QAudioDecoder>

#include "IAudioEncoder.h"
#include "Job.h"

class AudioJob : public QObject {
  Q_OBJECT
public:
  AudioJob(const Job& job, QObject *parent = nullptr);
  ~AudioJob();

  QString message() const;
  uint64_t numTimeSamples() const;
  QString outputFilePath() const;

  bool start();

private slots:
  void decodingBufferReady();
  void decodingError(QAudioDecoder::Error error);
  void decodingFinished();

private:
  void appendErrorMessage(const QString& msg);
  void appendInfoMessage(const QString& msg);

  QAudioDecoder _decoder;
  AudioEncoderPtr _encoder;
  Job _job;
  QString _message;
  uint64_t _numTimeSamples;
  QString _outputFilePath;

signals:
  void done();
};

#endif // AUDIOJOB_H
