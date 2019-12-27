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

#ifndef MPEG4_ASC_H
#define MPEG4_ASC_H

#include <cstdint>

/*
 * NOTE:
 * A minimal AudioSpecificConfig().
 * cf. ISO 14496-3 Subpart 1 (AKA "MPEG-4 Audio")
 * Chapter 3: Interface to MPEG-4 Systems
 * Document W2203
 */

namespace mpeg4_asc {

  enum class AudioObjectType : uint16_t {
    Null     = 0b0000'0000'0000'0000,
    AAC_main = 0b0000'1000'0000'0000,
    AAC_LC   = 0b0001'0000'0000'0000,
    AAC_SSR  = 0b0001'1000'0000'0000,
    AAC_LTP  = 0b0010'0000'0000'0000,
    Rsvd31   = 0b1111'1000'0000'0000
  };

  enum class SamplingFrequencyIndex : uint16_t {
    Index_96000 = 0b0000'0000'0000'0000,
    Index_88200 = 0b0000'0000'1000'0000,
    Index_64000 = 0b0000'0001'0000'0000,
    Index_48000 = 0b0000'0001'1000'0000,
    Index_44100 = 0b0000'0010'0000'0000,
    Index_32000 = 0b0000'0010'1000'0000,
    Index_24000 = 0b0000'0011'0000'0000,
    Index_22050 = 0b0000'0011'1000'0000,
    Index_16000 = 0b0000'0100'0000'0000,
    Index_12000 = 0b0000'0100'1000'0000,
    Index_11025 = 0b0000'0101'0000'0000,
    Index_8000  = 0b0000'0101'1000'0000,
    Index_rsvd1 = 0b0000'0110'0000'0000,
    Index_rsvd2 = 0b0000'0110'1000'0000,
    Index_rsvd3 = 0b0000'0111'0000'0000,
    Escape      = 0b0000'0111'1000'0000
  };

  enum class ChannelConfiguration : uint16_t {
    Specific     = 0b0000'0000'0000'0000,
    Config_1     = 0b0000'0000'0000'1000,
    Config_2     = 0b0000'0000'0001'0000,
    Config_3     = 0b0000'0000'0001'1000,
    Config_4     = 0b0000'0000'0010'0000,
    Config_5     = 0b0000'0000'0010'1000,
    Config_5p1   = 0b0000'0000'0011'0000,
    Config_7p1   = 0b0000'0000'0011'1000,
    Config_rsvd1 = 0b0000'0000'0100'0000,
    Config_rsvd2 = 0b0000'0000'0100'1000,
    Config_rsvd3 = 0b0000'0000'0101'0000,
    Config_rsvd4 = 0b0000'0000'0101'1000,
    Config_rsvd5 = 0b0000'0000'0110'0000,
    Config_rsvd6 = 0b0000'0000'0110'1000,
    Config_rsvd7 = 0b0000'0000'0111'0000,
    Config_rsvd8 = 0b0000'0000'0111'1000
  };

} // namespace mpeg4_asc

#endif // MPEG4_ASC_H
