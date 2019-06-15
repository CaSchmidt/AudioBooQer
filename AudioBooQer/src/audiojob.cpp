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

#include <QtCore/QFile>

#include "audiojob.h"

#define HAVE_AAC

#ifdef HAVE_AAC
# include "aacencoder.h"
#else
# include "rawencoder.h"
#endif

////// public ////////////////////////////////////////////////////////////////

AudioJob::AudioJob(const Job& job, QObject *parent)
  : QObject(parent)
  , _decoder(this)
  , _encoder()
  , _job(job)
  , _message()
{
  // Signals & Slots /////////////////////////////////////////////////////////

  connect(&_decoder, &QAudioDecoder::bufferReady,
          this, &AudioJob::decodingBufferReady);
  connect(&_decoder, QOverload<QAudioDecoder::Error>::of(&QAudioDecoder::error),
          this, &AudioJob::decodingError);
  connect(&_decoder, &QAudioDecoder::finished,
          this, &AudioJob::decodingFinished);
}

AudioJob::~AudioJob()
{
}

QString AudioJob::message() const
{
  return _message;
}

uint64_t AudioJob::numTimeSamples() const
{
  return _encoder->numTimeSamples();
}

QString AudioJob::outputFilePath() const
{
  return _encoder->outputFileName();
}

bool AudioJob::start()
{
  // (1) Initialize decoder //////////////////////////////////////////////////

  _decoder.setAudioFormat(_job.format);
  if( _decoder.error() != QAudioDecoder::NoError ) {
    appendErrorMessage(_decoder.errorString());
    return false;
  }

  // (2) Initialize encoder //////////////////////////////////////////////////

  try {
#ifdef HAVE_AAC
    _encoder = std::make_unique<AacEncoder>();
#else
    _encoder = std::make_unique<RawEncoder>();
#endif
  } catch(...) {
    _encoder.reset();
  }

  if( !_encoder ) {
    appendErrorMessage(QStringLiteral("IAudioEncoder is <nullptr>!"));
    return false;
  }

  if( !_encoder->initialize(_job.format, _job.outputDirPath, _job.title) ) {
    appendErrorMessage(QStringLiteral("IAudioEncoder::initialize() failed!"));
    return false;
  }

  // (3) Decode -> Encode ////////////////////////////////////////////////////

  _decoder.setSourceFilename(_job.inputFiles.takeFirst());
  _decoder.start();

  return true;
}

////// private slots /////////////////////////////////////////////////////////

void AudioJob::decodingBufferReady()
{
  const QAudioBuffer buffer = _decoder.read();
  if( !_encoder->encode(buffer) ) {
    _decoder.stop();
    appendErrorMessage(QStringLiteral("IAudioEncoder::encode() failed!"));
    emit done();
    return;
  }
}

void AudioJob::decodingError(QAudioDecoder::Error /*error*/)
{
  _decoder.stop();
  appendErrorMessage(_decoder.errorString());
  emit done();
}

void AudioJob::decodingFinished()
{
  _decoder.stop();
  appendInfoMessage(QStringLiteral("+ %1").arg(_decoder.sourceFilename()));

  if( _job.renameInput ) {
    QFile::rename(_decoder.sourceFilename(), _decoder.sourceFilename() + QStringLiteral(".done"));
  }

  if( _job.inputFiles.isEmpty() ) {
    if( !_encoder->flush() ) {
      appendErrorMessage(QStringLiteral("IAudioEncoder::flush() failed!"));
    } else {
      appendInfoMessage(QStringLiteral("= %1").arg(_encoder->outputFileName()));
    }
    emit done();
  } else {
    _decoder.setSourceFilename(_job.inputFiles.takeFirst());
    _decoder.start();
  }
}

////// private ///////////////////////////////////////////////////////////////

void AudioJob::appendErrorMessage(const QString& msg)
{
  _message.append(QStringLiteral("ERROR: %1\n").arg(msg));
}

void AudioJob::appendInfoMessage(const QString& msg)
{
  _message.append(QStringLiteral("INFO: %1\n").arg(msg));
}
