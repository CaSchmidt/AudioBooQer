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

#include <chrono>
#include <numeric>
#include <sstream>

#include <mp4v2/mp4v2.h>

#include <cs/IO/File.h>
#include <cs/Logging/OutputContext.h>
#include <cs/Text/StringUtil.h>

#include "Output.h"

#include "AdtsParser.h"
#include "Mpeg4Audio.h"

////// Asserts ///////////////////////////////////////////////////////////////

static_assert(mpeg4::numSamplesPerAacFrame == 1024);

////// Types /////////////////////////////////////////////////////////////////

using Durations = std::vector<MP4Duration>;

////// Private ///////////////////////////////////////////////////////////////

namespace priv {

  MP4Duration adtsFrameCount(const std::filesystem::path& filename, uint16_t *globalAsc,
                             const cs::OutputContext& ctx)
  {
    ctx.logText(u8"Reading ADTS file \"" + filename.generic_u8string() + u8"\".");

    // (1) Read ADTS file ////////////////////////////////////////////////////

    cs::File file;
    file.open(filename);
    cs::Buffer buffer = file.readAll();
    if( buffer.empty() ) {
      ctx.logError(u8"Unable to read ADTS file \"" + filename.generic_u8string() + u8"\"!");
      return 0;
    }

    AdtsParser adts(std::move(buffer));

    // (2) Initialize reference ASC //////////////////////////////////////////

    uint16_t refAsc = globalAsc != nullptr  &&  *globalAsc != 0
        ? *globalAsc
        : 0;

    // (3) Count & validate frames ///////////////////////////////////////////

    MP4Duration count{0};
    while( adts.hasFrame() ) {
      const uint16_t asc = adts.mpeg4AudioSpecificConfig();

      // (3.1) Set new reference /////////////////////////////////////////////

      if( refAsc == 0 ) {
        refAsc = asc;
      }

      // (3.2) Count /////////////////////////////////////////////////////////

      count++;

      // (3.3) Validate //////////////////////////////////////////////////////

      if( adts.aacFrameCount() != 1 ) {
        ctx.logError(u8"Invalid AAC frame count detected!");
        return 0;
      }

      if( refAsc == 0  ||  asc != refAsc ) {
        ctx.logError(u8"Invalid AudioSpecificConfig detected!");
        return 0;
      }

      // (3.4) Next //////////////////////////////////////////////////////////

      adts.nextFrame();
    }

    // (4) Update global ASC reference ///////////////////////////////////////

    if( globalAsc != nullptr  &&  *globalAsc == 0 ) {
      *globalAsc = refAsc;
    }

    // Done! /////////////////////////////////////////////////////////////////

    return count;
  }

  std::u8string formatAsc(const uint16_t asc)
  {
    std::ostringstream output;

    const uint16_t aot = mpeg4::audioObjectTypeFromASC(asc);
    {
      if(        aot == 0 ) {
        output << "Null";
      } else if( aot == 1 ) {
        output << "AAC Main";
      } else if( aot == 2 ) {
        output << "AAC LC";
      } else if( aot == 3 ) {
        output << "AAC SSR";
      } else if( aot == 4 ) {
        output << "AAC LTP";
      } else {
        output << "???";
      }
    }
    output << ", ";

    const uint32_t freq = mpeg4::samplingFrequencyFromASC(asc);
    {
      output << freq << "Hz";
    }
    output << ", ";

    const uint16_t ch = mpeg4::channelConfigurationFromASC(asc);
    {
      if(        ch == 1 ) {
        output << "Mono";
      } else if( ch == 2 ) {
        output << "Stereo";
      } else {
        output << ch << " channels";
      }
    }

    return cs::toUtf8String(output.str());
  }

  void printBinder(const BookBinder& binder,
                   const Durations& durations, const uint32_t timeScale,
                   const cs::OutputContext& ctx)
  {
    using std::chrono::minutes;
    using std::chrono::seconds;

    size_t i{0};
    for(const BookBinderChapter& chapter : binder) {
      const seconds dur(durations[i]/MP4Duration(timeScale));
      const minutes min = std::chrono::duration_cast<minutes>(dur);
      const seconds sec = std::chrono::duration_cast<seconds>(dur % minutes(1));

      const std::u8string filename = chapter.second.filename().generic_u8string();
      const std::u8string    title = chapter.first;

      std::ostringstream output;

      output << "Chapter #" << (i + 1) << ": " << cs::toString(filename) << ", ";
      output << "\"" << cs::toString(title) << "\"";
      output << ", " << min.count() << "m" << sec.count() << "s";

      i++;

      ctx.logText(cs::toUtf8String(output.str()));
    }
  }

  bool writeAdtsSample(MP4FileHandle file, const MP4TrackId trackId,
                       const std::filesystem::path& filename, const cs::OutputContext& ctx)
  {
    ctx.logText(u8"Writing ADTS file \"" + filename.generic_u8string() + u8"\".");

    // (1) Read ADTS file ////////////////////////////////////////////////////

    cs::File sampleFile;
    sampleFile.open(filename);
    cs::Buffer buffer = sampleFile.readAll();
    if( buffer.empty() ) {
      ctx.logError(u8"Unable to read ADTS file \"" + filename.generic_u8string() + u8"\"!");
      return false;
    }

    AdtsParser adts(std::move(buffer));

    // (2) Write frames //////////////////////////////////////////////////////

    while( adts.hasFrame() ) {
      if( !MP4WriteSample(file, trackId,
                          adts.frameData(), uint32_t(adts.frameSize())) ) {
        ctx.logError(u8"Unable to write AAC frame!");
        return false;
      }

      adts.nextFrame();
    }

    return true;
  }

} // namespace priv

////// Public ////////////////////////////////////////////////////////////////

bool outputAdtsBinder(const std::filesystem::path& filename, const BookBinder& binder,
                      const cs::OutputContext& ctx,
                      const std::u8string& language)
{
  // (0) Sanity check ////////////////////////////////////////////////////////

  if( binder.empty() ) {
    ctx.logWarning(u8"Empty input!");
    return false;
  }

  ctx.setProgressRange(0, int(binder.size()));

  // (1) Validate & accumulate ADTS frames ///////////////////////////////////

  Durations durations{};
  uint16_t     refAsc{0};
  {
    try {
      durations.resize(std::size_t(binder.size()));
    } catch(...) {
      ctx.logError(u8"std::vector<>::resize() failed!");
      return false;
    }

    for(std::size_t i = 0; const BookBinderChapter& chapter : binder) {
      durations[i] = priv::adtsFrameCount(chapter.second, &refAsc, ctx);
      if( durations[i] == 0 ) {
        return false;
      } else {
        durations[i] *= MP4Duration(mpeg4::numSamplesPerAacFrame);
      }
      i++;

      if( refAsc == 0 ) {
        ctx.logError(u8"Invalid AudioSpecificConfig detected!");
        return false;
      }

      ctx.setProgressValue(int(i - 1));
    }
  } // End durations

  const MP4Duration duration = std::accumulate(durations.begin(), durations.end(), MP4Duration{0});

  // (2) Extract & validate time scale ///////////////////////////////////////

  const uint32_t timeScale = mpeg4::samplingFrequencyFromASC(refAsc);
  if( timeScale == 0 ) {
    ctx.logError(u8"Invalid time scale!");
    return false;
  }

  ctx.logText(u8"Detected format: " + priv::formatAsc(refAsc));
  priv::printBinder(binder, durations, timeScale, ctx);

  // (3) Create MP4 file /////////////////////////////////////////////////////

  const char *brand0 = "M4B ";
  const char *brand1 = "isom";
  const char *brand2 = "mp42"; // required for a valid MP4 file, cf. ISO 14496-14 "4 File Identification"
  const char *brands[] = { brand0, brand1, brand2 };
  char **compBrands = const_cast<char**>(brands);

  const MP4FileHandle file =
      MP4CreateEx(cs::CSTR(filename.generic_u8string()), 0, 1, 0, compBrands[0], 0, compBrands, 3);
  if( file == MP4_INVALID_FILE_HANDLE ) {
    ctx.logError(u8"Unable to create output file \"" + filename.generic_u8string() + u8"\"!");
    return false;
  }

  // (4) Set timing information for file /////////////////////////////////////

  MP4SetDuration(file, duration);
  MP4SetTimeScale(file, timeScale);

  // (5) Create audio track //////////////////////////////////////////////////

  const MP4TrackId auTrackId =
      MP4AddAudioTrack(file, timeScale, mpeg4::numSamplesPerAacFrame, MP4_MPEG4_AUDIO_TYPE);
  if( auTrackId == MP4_INVALID_TRACK_ID ) {
    ctx.logError(u8"Unable to create audio track!");
    MP4Close(file);
    return false;
  }

  // (5.1) Set track's flags /////////////////////////////////////////////////

  /*
   * Flags (tkhd.flags), cf. ISO 14496-12 "8.3.2 Track Header Box":
   *
   * 0x1: Track_enabled
   * 0x2: Track_in_movie
   * 0x4: Track_in_preview
   * 0x8: Track_size_is_aspect_ratio
   */
  if( !MP4SetTrackIntegerProperty(file, auTrackId, "tkhd.flags", 0xF) ) {
    ctx.logError(u8"Unable to set audio flags!");
    MP4Close(file);
    return false;
  }

  // (6) Write AudioSpecificConfig ///////////////////////////////////////////

  if( !MP4SetTrackESConfiguration(file, auTrackId,
                                  reinterpret_cast<const uint8_t*>(&refAsc), sizeof(uint16_t)) ) {
    ctx.logError(u8"Unable to write AudioSpecificConfig!");
    MP4Close(file);
    return false;
  }

  // (7) Write chapters to MP4 file //////////////////////////////////////////

  for(std::size_t i = 0; const BookBinderChapter& chapter : binder) {
    if( !priv::writeAdtsSample(file, auTrackId, chapter.second, ctx) ) {
      MP4Close(file);
      return false;
    }
    i++;

    ctx.setProgressValue(int(i - 1));
  }

  // (8) Creater chapter track ///////////////////////////////////////////////

  const MP4TrackId chTrackId =
      MP4AddChapterTextTrack(file, auTrackId);
  if( chTrackId == MP4_INVALID_TRACK_ID ) {
    ctx.logError(u8"Unable to create chapter track!");
    MP4Close(file);
    return false;
  }

  // (8.1) Set track's flags /////////////////////////////////////////////////

  /*
   * Flags (tkhd.flags), cf. ISO 14496-12 "8.3.2 Track Header Box":
   *
   * 0x1: Track_enabled
   * 0x2: Track_in_movie
   * 0x4: Track_in_preview
   * 0x8: Track_size_is_aspect_ratio
   */
  if( !MP4SetTrackIntegerProperty(file, chTrackId, "tkhd.flags", 0xF) ) {
    ctx.logError(u8"Unable to set chapter flags!");
    MP4Close(file);
    return false;
  }

  // (8.2) Set track's language //////////////////////////////////////////////

  if( language.size() == 3 ) { // cf. ISO 639-2/T
    if( !MP4SetTrackLanguage(file, chTrackId, cs::CSTR(language)) ) {
      ctx.logError(u8"Unable to set chapter language!");
      MP4Close(file);
      return false;
    }
  }

  // (9) Create chapters /////////////////////////////////////////////////////

  for(std::size_t i = 0; const BookBinderChapter& chapter : binder) {
    MP4AddChapter(file, chTrackId, durations[i], cs::CSTR(chapter.first));
    i++;

    ctx.setProgressValue(int(i));
  }

  // Done! ///////////////////////////////////////////////////////////////////

  MP4Close(file);

  return true;
}
