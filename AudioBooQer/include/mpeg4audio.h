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

#ifndef MPEG4AUDIO_H
#define MPEG4AUDIO_H

#include <cstdint>

#include <array>

namespace mpeg4 {

  // Audio Object Type: 5bits
  enum class AudioObjectType : uint16_t {
    Null = 0,
    AAC_Main,
    AAC_LC,   // Low Complexity
    AAC_SSR,  // Scalable Sample Rate
    AAC_LTP   // Long Term Prediction
  };

  // ChannelConfiguration: 4bits
  /*
   * C  -> Center
   * F  -> Front
   * L  -> Left
   * Ri -> Right
   * Rr -> Rear
   * S  -> Surround
   */
  enum class ChannelConfiguration : uint16_t {
    Config_Specific = 0, // cf. AudioSpecificConfig
    Config_1,   // CF
    Config_2,   // LF+RiF
    Config_3,   // CF+LF+RiF
    Config_4,   // CF+LF+RiF+SRr
    Config_5,   // CF+LF+RiF+LSRr+RiSRr
    Config_5p1, // 5.1 Surround
    Config_7p1, // 7.1 Surround
    Config_Reserved8,
    Config_Reserved9,
    Config_Reserved10,
    Config_Reserved11,
    Config_Reserved12,
    Config_Reserved13,
    Config_Reserved14,
    Config_Reserved15
  };

  extern const std::array<uint32_t,16> SamplingFrequencyData;

  // SamplingFrequencyIndex: 4bits
  enum class SamplingFrequencyIndex : uint16_t {
    Index_96000 = 0,
    Index_88200,
    Index_64000,
    Index_48000,
    Index_44100,
    Index_32000,
    Index_24000,
    Index_22050,
    Index_16000,
    Index_12000,
    Index_11025,
    Index_8000,
    Index_Reserved12,
    Index_Reserved13,
    Index_Reserved14,
    Index_Escape // cf. AudioSpecificConfig
  };

  uint16_t createAudioSpecificConfig(const uint16_t aot, const uint16_t channels, const uint32_t freq);

  uint16_t audioObjectTypeFromASC(const uint16_t asc);
  uint16_t channelConfigurationFromASC(const uint16_t asc);
  uint16_t samplingFrequencyIndexFromASC(const uint16_t asc);
  uint32_t samplingFrequencyFromASC(const uint16_t asc);

} // namespace mpeg4

#endif // MPEG4AUDIO_H
