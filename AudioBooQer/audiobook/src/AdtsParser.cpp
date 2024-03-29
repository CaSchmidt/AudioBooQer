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

#include <cs/Core/Endian.h>

#include "AdtsParser.h"

#include "Mpeg4Audio.h"

////// public ////////////////////////////////////////////////////////////////

AdtsParser::AdtsParser(cs::Buffer&& buffer)
  : _buffer(std::move(buffer))
{
  readHeader();
}

AdtsParser::size_type AdtsParser::aacFrameCount() const
{
  return hasFrame()
      ? numberFrames()
      : 0;
}

const uint8_t *AdtsParser::frameData() const
{
  return hasFrame()
      ? _buffer.data() + _offset + headerSize()
      : nullptr;
}

AdtsParser::size_type AdtsParser::frameSize() const
{
  return hasFrame()
      ? adtsLength() - headerSize()
      : 0;
}

bool AdtsParser::hasFrame() const
{
  return isHeader()  &&  _offset + adtsLength() <= _buffer.size();
}

bool AdtsParser::isMpeg2Frame() const
{
  return hasFrame()  &&  (_header & Mpeg2) == Mpeg2;
}

bool AdtsParser::isMpeg4Frame() const
{
  return hasFrame()  &&  (_header & Mpeg2) == 0;
}

uint16_t AdtsParser::mpeg4AudioSpecificConfig() const
{
  return hasFrame()
      ? mpeg4::createAudioSpecificConfig(mpeg4AudioType(), mpeg4Channels(), mpeg4Frequency())
      : 0;
}

bool AdtsParser::nextFrame()
{
  _offset += adtsLength();
  readHeader();
  return hasFrame();
}

bool AdtsParser::reset()
{
  _offset = 0;
  readHeader();
  return hasFrame();
}

////// private ///////////////////////////////////////////////////////////////

AdtsParser::size_type AdtsParser::adtsLength() const
{
  return isHeader()
      ? (_header & AdtsLength) >> 21
      : 0;
}

AdtsParser::size_type AdtsParser::headerSize() const
{
  return isHeader()
      ? isNoProtection()
        ? 7
        : 9
      : 0;
}

bool AdtsParser::isHeader() const
{
  return (_header & Sync) == Sync;
}

bool AdtsParser::isNoProtection() const
{
  return isHeader()  &&  (_header & NoProtection) == NoProtection;
}

uint16_t AdtsParser::mpeg4AudioType() const
{
  return isHeader()
      ? static_cast<uint16_t>((_header & Mpeg4AudioType) >> 46) + 1
      : 0;
}

uint16_t AdtsParser::mpeg4Channels() const
{
  return isHeader()
      ? static_cast<uint16_t>((_header & Mpeg4Channels) >> 38)
      : 0;
}

uint16_t AdtsParser::mpeg4Frequency() const
{
  return isHeader()
      ? static_cast<uint16_t>((_header & Mpeg4Frequency) >> 42)
      : 0;
}

AdtsParser::size_type AdtsParser::numberFrames() const
{
  return isHeader()
      ? ((_header & NumberAacFrames) >> 8) + 1
      : 0;
}

bool AdtsParser::readHeader()
{
  _header = 0;

  if( _offset + sizeof(uint64_t) > _buffer.size() ) {
    return false;
  }

  _header = cs::fromBigEndian<uint64_t>(*reinterpret_cast<const uint64_t*>(_buffer.data() + _offset));

  return isHeader();
}
