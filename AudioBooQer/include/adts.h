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

#ifndef ADTS_H
#define ADTS_H

#include <cstdint>

#include <vector>

class AdtsParser {
public:
  using Buffer = std::vector<uint8_t>;
  using size_type = Buffer::size_type;

  AdtsParser(Buffer&& buffer);
  ~AdtsParser() noexcept = default;

  size_type frameCount() const;
  const uint8_t *frameData() const;
  size_type frameSize() const;
  bool hasFrame() const;
  bool isMpeg2Frame() const;
  bool isMpeg4Frame() const;
  bool nextFrame();
  bool reset();

private:
  AdtsParser() noexcept = delete;

  AdtsParser(const AdtsParser&) noexcept = delete;
  AdtsParser& operator=(const AdtsParser&) noexcept = delete;

  AdtsParser(AdtsParser&&) noexcept = delete;
  AdtsParser& operator=(AdtsParser&&) noexcept = delete;

  enum HeaderBits : uint64_t {
    Sync         = 0xFFF0000000000000,
    Mpeg2        = 0x0008000000000000,
    NoProtection = 0x0001000000000000,
    AdtsLength   = 0x00000003FFE00000,
    NumberFrames = 0x0000000000000300
  };

  size_type adtsLength() const;
  size_type headerSize() const;
  bool isHeader() const;
  bool isNoProtection() const;
  size_type numberFrames() const;
  bool readHeader();

  Buffer _buffer{};
  uint64_t _header{};
  size_type _offset{};
};

#endif // ADTS_H
