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

#include <bit>

#include <cs/Text/PrintUtil.h>

#include "RawEncoder.h"

////// public ////////////////////////////////////////////////////////////////

RawEncoder::RawEncoder()
  : _file()
{
}

RawEncoder::~RawEncoder()
{
  _file.close();
}

bool RawEncoder::encode(const void *data, const std::size_t size)
{
  if( !isValidData(data, size) ) {
    return false;
  }
  if( _file.write(data, size) != size ) {
    return false;
  }
  _numPcmFrames += uint64_t(size)/_bytesPerPcmFrame;
  return true;
}

bool RawEncoder::initialize(const AacFormat& format, const std::filesystem::path& outputFileName)
{
  if( _file.isOpen()  ||  !format.isValid() ) {
    return false;
  }
  if( !_file.open(outputFileName, cs::FileOpenFlag::Write) ) {
    return false;
  }
  _bytesPerPcmFrame = format.numBytesPerPcmFrame();
  _numPcmFrames     = 0;
  return _file.isOpen();
}

uint64_t RawEncoder::numPcmFrames() const
{
  return _numPcmFrames;
}

std::filesystem::path RawEncoder::outputSuffix(const AacFormat& format) const
{
  if( !format.isValid() ) {
    return std::string("raw");
  }
  return cs::sprint("%.%ch.s%.%Hz.raw",
                    std::endian::native == std::endian::big
                    ? "be"
                    : "le",
                    format.numChannels,
                    format.numBitsPerChannel,
                    format.numSamplesPerSecond);
}
