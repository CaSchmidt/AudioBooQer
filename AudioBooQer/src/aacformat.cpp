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

#include "aacformat.h"

bool AacFormat::isSupportedRate(const unsigned int rate) const
{
  for(unsigned int i = 0; i < numSupportedRates(); i++) {
    if( rate == supportedRates[i] ) {
      return true;
    }
  }
  return false;
}

bool AacFormat::isValid() const
{
  if( numBitsPerChannel != 16 ) {
    return false;
  }

  if( numChannels != 1  &&  numChannels != 2 ) {
    return false;
  }

  if( numSamplesPerAacFrame != 1024 ) {
    return false;
  }

  if( !isSupportedRate(numSamplesPerSecond) ) {
    return false;
  }

  return true;
}

AacFormat::operator QAudioFormat() const
{
  if( !isValid() ) {
    return QAudioFormat();
  }

  QAudioFormat result;
  result.setByteOrder(static_cast<QAudioFormat::Endian>(QSysInfo::ByteOrder));
  result.setChannelCount(static_cast<int>(numChannels));
  result.setCodec(QStringLiteral("audio/pcm"));
  result.setSampleRate(static_cast<int>(numSamplesPerSecond));
  result.setSampleSize(static_cast<int>(numBitsPerChannel));
  result.setSampleType(QAudioFormat::SignedInt);

  return result;
}