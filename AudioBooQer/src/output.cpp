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

  uint16_t createASC(const QAudioFormat& fmt)
  {
    using namespace mpeg4_asc;

    uint16_t result = 0;

    result |= toInt(AudioObjectType::AAC_LC);

    if(        fmt.sampleRate() == 8000 ) {
      result |= toInt(SamplingFrequencyIndex::Index_8000);
    } else if( fmt.sampleRate() == 11025 ) {
      result |= toInt(SamplingFrequencyIndex::Index_11025);
    } else if( fmt.sampleRate() == 12000 ) {
      result |= toInt(SamplingFrequencyIndex::Index_12000);
    } else if( fmt.sampleRate() == 16000 ) {
      result |= toInt(SamplingFrequencyIndex::Index_16000);
    } else if( fmt.sampleRate() == 22050 ) {
      result |= toInt(SamplingFrequencyIndex::Index_22050);
    } else if( fmt.sampleRate() == 24000 ) {
      result |= toInt(SamplingFrequencyIndex::Index_24000);
    } else if( fmt.sampleRate() == 32000 ) {
      result |= toInt(SamplingFrequencyIndex::Index_32000);
    } else if( fmt.sampleRate() == 44100 ) {
      result |= toInt(SamplingFrequencyIndex::Index_44100);
    } else if( fmt.sampleRate() == 48000 ) {
      result |= toInt(SamplingFrequencyIndex::Index_48000);
    } else if( fmt.sampleRate() == 64000 ) {
      result |= toInt(SamplingFrequencyIndex::Index_64000);
    } else if( fmt.sampleRate() == 88200 ) {
      result |= toInt(SamplingFrequencyIndex::Index_88200);
    } else if( fmt.sampleRate() == 96000 ) {
      result |= toInt(SamplingFrequencyIndex::Index_96000);
    } else {
      return 0;
    }

    if(        fmt.channelCount() == 1 ) {
      result |= toInt(ChannelConfiguration::Config_1);
    } else if( fmt.channelCount() == 2 ) {
      result |= toInt(ChannelConfiguration::Config_2);
    } else {
      return 0;
    }

    return qToBigEndian(result);
  }

} // namespace priv

void writeOutput(const QString& fileName, const QAudioFormat& format, JobResults results)
{
  qSort(results);

  MP4Duration duration = 0;
  for(const JobResult& result : results) {
    duration += result.numTimeSamples;
  }

  const uint32_t timeScale = static_cast<uint32_t>(format.sampleRate());

  const MP4FileHandle output = MP4CreateEx(fileName.toUtf8().constData(), 0,
                                           1, 0,
                                           const_cast<char*>("M4B "), 0);
  if( output == MP4_INVALID_FILE_HANDLE ) {
    return;
  }

  MP4SetDuration(output, duration);
  MP4SetTimeScale(output, timeScale);

  const MP4TrackId auTrackId = MP4AddAudioTrack(output,
                                                timeScale, MP4_INVALID_DURATION,
                                                MP4_MPEG4_AUDIO_TYPE);
  if( auTrackId == MP4_INVALID_TRACK_ID ) {
    MP4Close(output);
  }
  MP4SetTrackIntegerProperty(output, auTrackId, "tkhd.flags", 15);

  const uint16_t asc = priv::createASC(format);
  if( asc != 0 ) {
    MP4SetTrackESConfiguration(output, auTrackId,
                               reinterpret_cast<const uint8_t*>(&asc), sizeof(uint16_t));
  }

  for(const JobResult& result : results) {
    QFile file(result.outputFilePath);
    if( !file.open(QIODevice::ReadOnly) ) {
      continue;
    }

    const QByteArray data = file.readAll();
    file.close();

    MP4WriteSample(output, auTrackId,
                   reinterpret_cast<const uint8_t*>(data.constData()), static_cast<uint32_t>(data.size()),
                   result.numTimeSamples);
  }

  const MP4TrackId chTrackId = MP4AddChapterTextTrack(output, auTrackId);
  if( chTrackId == MP4_INVALID_TRACK_ID ) {
    MP4Close(output);
    return;
  }
  MP4SetTrackIntegerProperty(output, chTrackId, "tkhd.flags", 15);

  for(const JobResult& result : results) {
    MP4AddChapter(output, chTrackId,
                  result.numTimeSamples, result.title.toUtf8().constData());
  }

  /*
  const MP4Tags *tags = MP4TagsAlloc();
  if( tags != nullptr  &&  MP4TagsFetch(tags, output) ) {
    MP4TagsSetName(tags, "name");
    MP4TagsSetArtist(tags, "artist");
    MP4TagsSetAlbumArtist(tags, "album artist");
    MP4TagsSetAlbum(tags, "album");
    MP4TagsSetGenre(tags, "genre");

    MP4TagsStore(tags, output);
    MP4TagsFree(tags);
  }
  */

  MP4Close(output);
}