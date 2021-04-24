/****************************************************************************
** Copyright (c) 2021, Carsten Schmidt. All rights reserved.
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

#include <vector>

#include <QtCore/QFile>

#include <csUtil/csILogger.h>
#include <csUtil/csQStringUtil.h>

#include <miniaudio_lib.h>

#include "MiniJob.h"

#include "AacEncoder.h"

////// Private ///////////////////////////////////////////////////////////////

namespace priv {

  bool createDecoder(ma_decoder *decoder, const QString& filename, const AacFormat& format)
  {
    ma_decoder_config config = ma_decoder_config_init(ma_format_s16,
                                                      format.numChannels,
                                                      format.numSamplesPerSecond);
    // TODO: Configure resampler...

#ifdef Q_OS_WINDOWS
    const ma_result result = ma_decoder_init_file_w(filename.toStdWString().data(), &config, decoder);
#else
    const ma_result result = ma_decoder_init_file(filename.toUtf8().constData(), &config, decoder);
#endif
    return result == MA_SUCCESS;
  }

} // namespace priv

////// Public ////////////////////////////////////////////////////////////////

JobResult executeMiniJob(const Job& job)
{
  constexpr std::size_t BUFFER_SIZE = 1024*1024;

  const std::size_t numBytesPerPcmFrame = job.format.numBytesPerPcmFrame();

  QString message;

  // (1) Initialize Buffer ///////////////////////////////////////////////////

  std::vector<uint8_t> buffer;
  try {
    buffer.resize(BUFFER_SIZE);
  } catch(...) {
    job.logger->logError(u8"Unable to allocate buffer!");
    return JobResult();
  }

  const ma_uint64 numPcmFramesBuffered = ma_uint64(buffer.size()/numBytesPerPcmFrame);

  // (2) Initialize Encoder //////////////////////////////////////////////////

  AudioEncoderPtr encoder;
  try {
    encoder = std::make_unique<AacEncoder>();
  }  catch(...) {
    encoder.reset();
  }
  if( !encoder ) {
    job.logger->logError(u8"IAudioEncoder is <nullptr>!");
    return JobResult();
  }

  const QString outputFilePath = job.outputFilePath(encoder.get());

  if( !encoder->initialize(job.format, cs::toUtf8String(outputFilePath)) ) {
    job.logger->logError(u8"IAudioEncoder::initialize() failed!");
    return JobResult();
  }

  // (3) Encode //////////////////////////////////////////////////////////////

  for(const QString& filename : job.inputFiles) {
    // (3.1) Create Decoder //////////////////////////////////////////////////

    ma_decoder decoder;
    if( !priv::createDecoder(&decoder, filename, job.format) ) {
      job.logger->logError(u8"ma_decoder_init_file() failed!");
      return JobResult();
    }

    // (3.2) Decode & Encode /////////////////////////////////////////////////

    while( true ) {
      const ma_uint64 numPcmFramesRead =
          ma_decoder_read_pcm_frames(&decoder, buffer.data(), numPcmFramesBuffered);

      if( numPcmFramesRead > 0 ) {
        encoder->encode(buffer.data(),
                        std::size_t(numPcmFramesRead)*numBytesPerPcmFrame);
      }

      if( numPcmFramesRead < numPcmFramesBuffered ) {
        break; // Decoder is depleted!
      }
    }

    // (3.3) Release Decoder /////////////////////////////////////////////////

    if( ma_decoder_uninit(&decoder) != MA_SUCCESS ) {
      job.logger->logError(u8"ma_decoder_uninit() failed!");
      return JobResult();
    }

    // (3.4) Output //////////////////////////////////////////////////////////

    if( job.renameInput ) {
      QFile::rename(filename, filename + QStringLiteral(".done"));
    }

    message.append(QStringLiteral("INFO: + %1\n").arg(filename));
  }

  // (4) Flush Encoder ///////////////////////////////////////////////////////

  if( !encoder->flush() ) {
    job.logger->logError(u8"IAudioEncoder::flush() failed!");
    return JobResult();
  }

  const uint64_t numPcmFrames = encoder->numPcmFrames();
  encoder.reset(); // NOTE: Set 'encoder' to <nullptr> & close output file!

  message.append(QStringLiteral("INFO: = %1\n").arg(outputFilePath));
  job.logger->logText(cs::toUtf8String(message));

  // (5) Done! ///////////////////////////////////////////////////////////////

  JobResult result;
  result.numPcmFrames   = numPcmFrames;
  result.outputFilePath = outputFilePath;
  result.position       = job.position;
  result.title          = job.title;

  return result;
}
