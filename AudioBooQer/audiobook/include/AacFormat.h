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

#ifndef AACFORMAT_H
#define AACFORMAT_H

/*
 * NOTE:
 * FDK AAC seems to operate on native endian, signed 16bit integers ONLY!
 * (cf. INT_PCM, libSYS/include/machine_type.h)
 */

struct AacFormat {
  AacFormat() noexcept = default;

  bool isSupportedRate(const unsigned int rate) const;
  bool isValid() const;

  unsigned int numBytesPerChannel() const;
  unsigned int numBytesPerTimeSample() const;

  unsigned int numBitsPerChannel{};
  unsigned int numChannels{};
  unsigned int numSamplesPerAacFrame{1024};
  unsigned int numSamplesPerSecond{};

  static constexpr unsigned int supportedRates[] = {
    8000,
    11025,
    22050,
    32000,
    44100,
    48000,
    96000
  };

  static constexpr unsigned int numSupportedRates()
  {
    return sizeof(supportedRates)/sizeof(unsigned int);
  }
};

#endif // AACFORMAT_H
