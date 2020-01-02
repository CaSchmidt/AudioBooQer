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

#include <QtCore/QtEndian>

#include "mpeg4audio.h"

namespace mpeg4 {

  ////// Macros //////////////////////////////////////////////////////////////

#define ASC_SHIFT_AOT        11
#define ASC_SHIFT_CHANNELS    3
#define ASC_SHIFT_FREQUENCY   7

  ////// Public //////////////////////////////////////////////////////////////

  const std::array<uint32_t,16> SamplingFrequencyData{
    {
      96000,
      88200,
      64000,
      48000,
      44100,
      32000,
      24000,
      22050,
      16000,
      12000,
      11025,
      8000,
      0xFFFFFFFF,
      0xFFFFFFFF,
      0xFFFFFFFF,
      0xFFFFFFFF
    }
  };

  uint16_t createAudioSpecificConfig(const uint16_t aot, const uint16_t channels, const uint32_t freq)
  {
    // Limit type to AAC and limit channels to defined range!
    if( aot > 4  ||  channels > 7 ) {
      return 0;
    }
    uint16_t index = freq > 11
        ? 12
        : static_cast<uint16_t>(freq);
    if( index > 11 ) {
      for(std::size_t i = 0; i < 12; i++) {
        if( freq == SamplingFrequencyData[i] ) {
          index = static_cast<uint16_t>(i);
          break;
        }
      }
    }
    // Valid index found?
    if( index > 11 ) {
      return 0;
    }
    uint16_t asc = 0;
    asc |=      aot << ASC_SHIFT_AOT;
    asc |=    index << ASC_SHIFT_FREQUENCY;
    asc |= channels << ASC_SHIFT_CHANNELS;
    return qToBigEndian(asc);
  }

  uint16_t audioObjectTypeFromASC(const uint16_t asc)
  {
    return (qFromBigEndian(asc) >> ASC_SHIFT_AOT) & 0x1F;
  }

  uint16_t channelConfigurationFromASC(const uint16_t asc)
  {
    return (qFromBigEndian(asc) >> ASC_SHIFT_CHANNELS) & 0xF;
  }

  uint16_t samplingFrequencyIndexFromASC(const uint16_t asc)
  {
    return (qFromBigEndian(asc) >> ASC_SHIFT_FREQUENCY) & 0xF;
  }

  uint32_t samplingFrequencyFromASC(const uint16_t asc)
  {
    const uint16_t index = samplingFrequencyIndexFromASC(asc);
    if( index >= SamplingFrequencyData.size() ) {
      return 0;
    }
    return SamplingFrequencyData[index];
  }

} // namespace mpeg4
