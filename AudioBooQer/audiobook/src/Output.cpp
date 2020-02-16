/****************************************************************************
** Copyright (c) 2020, Carsten Schmidt. All rights reserved.
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

#include <numeric>

#include <csUtil/csFileIO.h>
#include <csUtil/csTextConverter.h>
#include <mp4v2/mp4v2.h>

#include "Output.h"

#include "AdtsParser.h"
#include "Mpeg4Audio.h"

////// Types /////////////////////////////////////////////////////////////////

using Durations = std::vector<MP4Duration>;

////// Private ///////////////////////////////////////////////////////////////

namespace priv {

  MP4Duration adtsDuration(const std::string& filename_utf8, const uint16_t refAsc,
                           const uint32_t numSamplesPerAacFrame)
  {
    // (1) Read ADTS file ////////////////////////////////////////////////////

    AdtsParser::Buffer buffer = csReadBinaryFile(filename_utf8);
    if( buffer.empty() ) {
      return 0;
    }

    AdtsParser adts(std::move(buffer));

    // (2) Count & validate frames ///////////////////////////////////////////

    MP4Duration duration{0};
    while( adts.hasFrame() ) {
      const uint16_t asc = adts.mpeg4AudioSpecificConfig();

      // (2.1) Count /////////////////////////////////////////////////////////

      duration++;

      // (2.2) Validate //////////////////////////////////////////////////////

      if( adts.aacFrameCount() != 1  ||  asc != refAsc ) {
        return 0;
      }

      // (2.3) Next //////////////////////////////////////////////////////////

      adts.nextFrame();
    }

    // (3) Compute total number of time samples //////////////////////////////

    duration *= static_cast<MP4Duration>(numSamplesPerAacFrame);

    // Done! /////////////////////////////////////////////////////////////////

    return duration;
  }

  bool writeAdtsSample(MP4FileHandle file, const MP4TrackId trackId, const std::string& filename_utf)
  {
    // (1) Read ADTS file ////////////////////////////////////////////////////

    AdtsParser::Buffer buffer = csReadBinaryFile(filename_utf);
    if( buffer.empty() ) {
      return false;
    }

    AdtsParser adts(std::move(buffer));

    // (2) Write frames //////////////////////////////////////////////////////

    while( adts.hasFrame() ) {
      if( !MP4WriteSample(file, trackId,
                          adts.frameData(),
                          static_cast<uint32_t>(adts.frameSize())) ) {
        return false;
      }

      adts.nextFrame();
    }

    return true;
  }

} // namespace priv

////// Public ////////////////////////////////////////////////////////////////

bool outputAdtsBinder(const std::string& filename_utf8, const BookBinder& binder,
                      const uint16_t refAsc, const uint32_t numSamplesPerAacFrame)
{
  // (0) Sanity check ////////////////////////////////////////////////////////

  if( binder.empty()  ||  refAsc == 0  ||  numSamplesPerAacFrame != 1024 ) {
    return false;
  }

  // (1) Extract time scale //////////////////////////////////////////////////

  const uint32_t timeScale = mpeg4::samplingFrequencyFromASC(refAsc);
  if( timeScale == 0 ) {
    return false;
  }

  // (2) Validate & cumulate ADTS frames /////////////////////////////////////

  Durations durations;
  {
    try {
      durations.resize(static_cast<std::size_t>(binder.size()));
    } catch(...) {
      return false;
    }

    std::size_t i{0};
    for(const BookBinderChapter& chapter : binder) {
      durations[i] = priv::adtsDuration(chapter.second, refAsc, numSamplesPerAacFrame);
      if( durations[i] == 0 ) {
        return false;
      }
      i++;
    }
  } // End durations

  const MP4Duration duration = std::accumulate(durations.begin(), durations.end(), MP4Duration{0});

  // (3) Create MP4 file /////////////////////////////////////////////////////

  const MP4FileHandle file =
      MP4CreateEx(filename_utf8.data(), 0, 1, 0, const_cast<char*>("M4B "), 0);
  if( file == MP4_INVALID_FILE_HANDLE ) {
    return false;
  }

  // (4) Set timing information for file /////////////////////////////////////

  MP4SetDuration(file, duration);
  MP4SetTimeScale(file, timeScale);

  // (5) Create audio track //////////////////////////////////////////////////

  const MP4TrackId auTrackId =
      MP4AddAudioTrack(file, timeScale, numSamplesPerAacFrame, MP4_MPEG4_AUDIO_TYPE);
  if( auTrackId == MP4_INVALID_TRACK_ID ) {
    MP4Close(file);
    return false;
  }

  // (6) Write AudioSpecificConfig ///////////////////////////////////////////

  MP4SetTrackESConfiguration(file, auTrackId,
                             reinterpret_cast<const uint8_t*>(&refAsc), sizeof(uint16_t));

  // (7) Write chapters to MP4 file //////////////////////////////////////////

  for(const BookBinderChapter& chapter : binder) {
    if( !priv::writeAdtsSample(file, auTrackId, chapter.second) ) {
      MP4Close(file);
      return false;
    }
  }

  // (8) Creater chapter track ///////////////////////////////////////////////

  const MP4TrackId chTrackId =
      MP4AddChapterTextTrack(file, auTrackId);
  if( chTrackId == MP4_INVALID_TRACK_ID ) {
    MP4Close(file);
    return false;
  }

  // (9) Create chapters /////////////////////////////////////////////////////

  Durations::const_iterator chDuration = durations.cbegin();
  for(const BookBinderChapter& chapter : binder) {
    MP4AddChapter(file, chTrackId, *chDuration, csUnicodeToUtf8(chapter.first).data());
    ++chDuration;
  }

  // Done! ///////////////////////////////////////////////////////////////////

  MP4Close(file);

  return true;
}
