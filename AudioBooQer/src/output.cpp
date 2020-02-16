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

#include <type_traits>

#include <mp4v2/mp4v2.h>

#include <QtCore/QFile>
#include <QtCore/QtEndian>

#include "output.h"

#include "AacEncoder.h"
#include "AdtsParser.h"
#include "mpeg4_asc.h"

namespace priv {

  template<typename E>
  using if_enum_t =
  typename std::enable_if<std::is_enum_v<E>,typename std::underlying_type<E>::type>::type;

  template<typename E>
  constexpr if_enum_t<E> toInt(const E& e)
  {
    return static_cast<if_enum_t<E>>(e);
  }

  uint16_t createASC(const AacFormat& fmt)
  {    
    using namespace mpeg4_asc;

    uint16_t result = 0;
    if( !fmt.isValid() ) {
      return result;
    }

    result |= toInt(AudioObjectType::AAC_LC);

    if(        fmt.numSamplesPerSecond == 8000 ) {
      result |= toInt(SamplingFrequencyIndex::Index_8000);
    } else if( fmt.numSamplesPerSecond == 11025 ) {
      result |= toInt(SamplingFrequencyIndex::Index_11025);
    } else if( fmt.numSamplesPerSecond == 12000 ) {
      result |= toInt(SamplingFrequencyIndex::Index_12000);
    } else if( fmt.numSamplesPerSecond == 16000 ) {
      result |= toInt(SamplingFrequencyIndex::Index_16000);
    } else if( fmt.numSamplesPerSecond == 22050 ) {
      result |= toInt(SamplingFrequencyIndex::Index_22050);
    } else if( fmt.numSamplesPerSecond == 24000 ) {
      result |= toInt(SamplingFrequencyIndex::Index_24000);
    } else if( fmt.numSamplesPerSecond == 32000 ) {
      result |= toInt(SamplingFrequencyIndex::Index_32000);
    } else if( fmt.numSamplesPerSecond == 44100 ) {
      result |= toInt(SamplingFrequencyIndex::Index_44100);
    } else if( fmt.numSamplesPerSecond == 48000 ) {
      result |= toInt(SamplingFrequencyIndex::Index_48000);
    } else if( fmt.numSamplesPerSecond == 64000 ) {
      result |= toInt(SamplingFrequencyIndex::Index_64000);
    } else if( fmt.numSamplesPerSecond == 88200 ) {
      result |= toInt(SamplingFrequencyIndex::Index_88200);
    } else if( fmt.numSamplesPerSecond == 96000 ) {
      result |= toInt(SamplingFrequencyIndex::Index_96000);
    } else {
      return 0;
    }

    if(        fmt.numChannels == 1 ) {
      result |= toInt(ChannelConfiguration::Config_1);
    } else if( fmt.numChannels == 2 ) {
      result |= toInt(ChannelConfiguration::Config_2);
    } else {
      return 0;
    }

    return qToBigEndian(result);
  }

  AdtsParser::Buffer readAdtsFile(const QString& filename)
  {
    using    Buffer = AdtsParser::Buffer;
    using size_type = AdtsParser::size_type;

    Buffer buffer;

    QFile file(filename);
    if( !file.open(QIODevice::ReadOnly) ) {
      return Buffer();
    }

    try {
      buffer.resize(static_cast<size_type>(file.size()), 0);
    } catch(...) {
      return Buffer();
    }

    const qint64 got = file.read(reinterpret_cast<char*>(buffer.data()), file.size());
    if( got != file.size() ) {
      return Buffer();
    }

    file.close();

    return buffer;
  }

  bool writeChapter(const MP4FileHandle file, const MP4TrackId track, const JobResult& chapter,
                    const AacFormat& format)
  {
    const unsigned int frameLength = format.numSamplesPerAacFrame;

    // (1) Make sure chapter aligns with frame length ////////////////////////

    if( chapter.numTimeSamples%frameLength != 0 ) {
      return false;
    }

    const uint64_t numChapterFrames = chapter.numTimeSamples/frameLength;

    // (2) Read chapter's ADTS file and count its frames /////////////////////

    AdtsParser::Buffer adtsBuffer = readAdtsFile(chapter.outputFilePath);
    if( adtsBuffer.empty() ) {
      return false;
    }

    AdtsParser adts(std::move(adtsBuffer));
    if( !adts.hasFrame() ) {
      return false;
    }

    uint64_t numAdtsFrames = 0;
    while( adts.hasFrame() ) {
      numAdtsFrames++;
      adts.nextFrame();
    }
    adts.reset();

    // (3) Make sure ADTS frames do not exceed chapter's frames //////////////

    /*
     * NOTE:
     * It has been observed that the FDK AAC encoder appends some additional
     * frames in the ADTS file, that do not reflect the projected number of
     * frames!
     * Therefore we will store all of these additional frames in the chapter's
     * last sample. Its duration will be set to the frame length, not accounting
     * for the additional frames!
     */

    if( numAdtsFrames < numChapterFrames  ||
        numAdtsFrames - numChapterFrames > 3 ) {
      return false;
    }

    // (4) Write all frames excluding the last one ///////////////////////////

    for(uint64_t i = 0; i < numChapterFrames - 1; i++) {
      MP4WriteSample(file, track,
                     adts.frameData(), static_cast<uint32_t>(adts.frameSize()),
                     frameLength);
      adts.nextFrame();
    }

    // (5) Pack remaining frames into last sample ////////////////////////////

    AdtsParser::Buffer remain;
    while( adts.hasFrame() ) {
      for(AdtsParser::size_type i = 0; i < adts.frameSize(); i++) {
        remain.push_back(adts.frameData()[i]);
      }

      adts.nextFrame();
    }

    MP4WriteSample(file, track,
                   remain.data(), static_cast<uint32_t>(remain.size()),
                   frameLength);

    return true;
  }

} // namespace priv

void writeBook(const QString& fileName, const AacFormat& format, JobResults results)
{
  qSort(results);

  // (0) Constants ///////////////////////////////////////////////////////////

  /*
   * duration      : The audiobook's/file's total duration.
   *                 Should correspond to the duration of the longest track.
   * sampleDuration: Fixed duration of one MP4 audio sample.
   * timeScale     : Elapsed time units per second.
   *
   * NOTE:
   * 'duration' and 'sampleDuration' are in 'timeScale' units!
   */

  MP4Duration duration = 0;
  for(const JobResult& result : results) {
    duration += result.numTimeSamples;
  }

  const MP4Duration sampleDuration = format.numSamplesPerAacFrame;
  const uint32_t         timeScale = static_cast<uint32_t>(format.numSamplesPerSecond);

  // (1) Create a MP4 file of the M4B brand //////////////////////////////////

  /*
   * NOTE:
   * cf. https://mp4ra.org/ for a list of various brands!
   */

  const MP4FileHandle output = MP4CreateEx(fileName.toUtf8().constData(), 0,
                                           1, 0,
                                           const_cast<char*>("M4B "), 0);
  if( output == MP4_INVALID_FILE_HANDLE ) {
    return;
  }

  // (2) Set timing information for MP4 file /////////////////////////////////

  MP4SetDuration(output, duration);
  MP4SetTimeScale(output, timeScale);

  // (3) Create audio track //////////////////////////////////////////////////

  /*
   * NOTE:
   * By convention audio tracks are required to have a fixed sample size (in
   * 'timeScale' units) to facilitate seeking inside the track!
   */

  const MP4TrackId auTrackId = MP4AddAudioTrack(output,
                                                timeScale, sampleDuration,
                                                MP4_MPEG4_AUDIO_TYPE);
  if( auTrackId == MP4_INVALID_TRACK_ID ) {
    MP4Close(output);
    return;
  }

  // (4) Write AudioSpecificConfig() structure for audio track ///////////////

  const uint16_t asc = priv::createASC(format);
  if( asc != 0 ) {
    MP4SetTrackESConfiguration(output, auTrackId,
                               reinterpret_cast<const uint8_t*>(&asc), sizeof(uint16_t));
  }

  // (5) Write all chapters into audio track /////////////////////////////////

  for(const JobResult& result : results) {
    priv::writeChapter(output, auTrackId, result, format);
  }

  // (6) Create track for chapter titles /////////////////////////////////////

  const MP4TrackId chTrackId = MP4AddChapterTextTrack(output, auTrackId);
  if( chTrackId == MP4_INVALID_TRACK_ID ) {
    MP4Close(output);
    return;
  }

  // (7) Write all chapter titles ////////////////////////////////////////////

  for(const JobResult& result : results) {
    MP4AddChapter(output, chTrackId,
                  result.numTimeSamples, result.title.toUtf8().constData());
  }

  MP4Close(output);
}
