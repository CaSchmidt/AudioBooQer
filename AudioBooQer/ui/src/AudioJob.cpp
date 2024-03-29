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

#include <cs/Core/QStringUtil.h>
#include <cs/Logging/ILogger.h>

#include "AudioJob.h"

#define HAVE_AAC

#ifdef HAVE_AAC
# include "AacEncoder.h"
#else
# include "RawEncoder.h"
#endif

////// Private ///////////////////////////////////////////////////////////////

namespace priv {

  QAudioFormat convert(const AacFormat& fmt)
  {
    if( !fmt.isValid() ) {
      return QAudioFormat();
    }

    QAudioFormat result;
    result.setByteOrder(static_cast<QAudioFormat::Endian>(QSysInfo::ByteOrder));
    result.setChannelCount(static_cast<int>(fmt.numChannels));
    result.setCodec(QStringLiteral("audio/pcm"));
    result.setSampleRate(static_cast<int>(fmt.numSamplesPerSecond));
    result.setSampleSize(static_cast<int>(fmt.numBitsPerChannel));
    result.setSampleType(QAudioFormat::SignedInt);

    return result;
  }

} // namespace priv

////// public ////////////////////////////////////////////////////////////////

AudioJob::AudioJob(const Job& job, QObject *parent)
  : QObject(parent)
  , _decoder(this)
  , _encoder()
  , _job(job)
  , _message()
  , _outputFilePath()
{
  // Signals & Slots /////////////////////////////////////////////////////////

  connect(&_decoder, &QAudioDecoder::bufferReady,
          this, &AudioJob::decodingBufferReady, Qt::DirectConnection);
  connect(&_decoder, QOverload<QAudioDecoder::Error>::of(&QAudioDecoder::error),
          this, &AudioJob::decodingError, Qt::DirectConnection);
  connect(&_decoder, &QAudioDecoder::finished,
          this, &AudioJob::decodingFinished, Qt::DirectConnection);
}

AudioJob::~AudioJob()
{
}

const IAudioEncoder *AudioJob::encoder() const
{
  return _encoder.get();
}

QString AudioJob::outputFilePath() const
{
  return _outputFilePath;
}

bool AudioJob::start()
{
  // (1) Initialize decoder //////////////////////////////////////////////////

  _decoder.setAudioFormat(priv::convert(_job.format));
  if( _decoder.error() != QAudioDecoder::NoError ) {
    _job.logger->logError(cs::toUtf8String(_decoder.errorString()));
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
    _job.logger->logError(u8"IAudioEncoder is <nullptr>!");
    return false;
  }

  _outputFilePath = _job.outputFilePath(_encoder.get());

  if( !_encoder->initialize(_job.format, cs::toUtf8String(_outputFilePath)) ) {
    _job.logger->logError(u8"IAudioEncoder::initialize() failed!");
    return false;
  }

  // (3) Decode -> Encode ////////////////////////////////////////////////////

  startDecode();

  return true;
}

////// private slots /////////////////////////////////////////////////////////

void AudioJob::decodingBufferReady()
{
  const QAudioBuffer buffer = _decoder.read();
  if( !_encoder->encode(buffer.data(), buffer.byteCount()) ) {
    _decoder.stop();
    _job.logger->logError(u8"IAudioEncoder::encode() failed!");
    emit done();
    return;
  }
}

void AudioJob::decodingError(QAudioDecoder::Error /*error*/)
{
  _decoder.stop();
  _job.logger->logError(cs::toUtf8String(_decoder.errorString()));
  emit done();
}

void AudioJob::decodingFinished()
{
  _decoder.stop();
  const QString filename = closeInput();
  appendInfoMessage(QStringLiteral("+ %1").arg(filename));

  if( _job.inputFiles.isEmpty() ) {
    if( !_encoder->flush() ) {
      _job.logger->logError(u8"IAudioEncoder::flush() failed!");
    } else {
      appendInfoMessage(QStringLiteral("= %1").arg(_outputFilePath));
    }
    _job.logger->logText(cs::toUtf8String(_message));
    emit done();
  } else {
    startDecode();
  }
}

////// private ///////////////////////////////////////////////////////////////

void AudioJob::appendInfoMessage(const QString& msg)
{
  _message.append(QStringLiteral("INFO: %1\n").arg(msg));
}

QString AudioJob::closeInput()
{
  const QString filename = inputFileName();
  if( _job.renameInput ) {
    QFile::rename(filename, filename + QStringLiteral(".done"));
  }
  return filename;
}

QString AudioJob::inputFileName() const
{
  return _decoder.sourceFilename();
}

bool AudioJob::startDecode()
{
  _decoder.setSourceFilename(_job.inputFiles.takeFirst());
  _decoder.start();
  return true;
}
