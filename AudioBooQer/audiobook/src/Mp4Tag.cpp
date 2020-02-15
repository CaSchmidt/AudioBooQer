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

#include <csUtil/csFileIO.h>
#include <csUtil/csStringUtil.h>
#include <csUtil/csTextConverter.h>
#include <mp4v2/mp4v2.h>

#include "Mp4Tag.h"

////// Public ////////////////////////////////////////////////////////////////

bool Mp4Tag::isValid() const
{
  return !filename.empty();
}

bool Mp4Tag::write() const
{
  if( !isValid() ) {
    return false;
  }

  const MP4Tags *tags = MP4TagsAlloc();
  if( tags == nullptr ) {
    return false;
  }

  { // Begin Conversion
#define OUTPUT(str,meta)                              \
  if( !str.empty() ) {                              \
  const std::string utf8 = csUnicodeToUtf8(str);  \
  MP4TagsSet##meta(tags, utf8.data());            \
  }
    OUTPUT(title,Album);
    OUTPUT(chapter,Name);
    OUTPUT(author,Artist);
    OUTPUT(albumArtist,AlbumArtist);
    OUTPUT(composer,Composer);
    OUTPUT(genre,Genre);
#undef OUTPUT
    {
      MP4TagTrack track;
      track.index = trackIndex;
      track.total = trackTotal;
      MP4TagsSetTrack(tags, &track);
    }
    {
      MP4TagDisk disk;
      disk.index = diskIndex;
      disk.total = diskTotal;
      MP4TagsSetDisk(tags, &disk);
    }
    {
      const std::vector<uint8_t> fileData = csReadBinaryFile(csUnicodeToUtf8(coverImageFilePath));
      if( !fileData.empty() ) {
        const bool is_jpeg =
            cs::endsWith(coverImageFilePath.data(), cs::MAX_SIZE_T, u".jpg", cs::MAX_SIZE_T, true)
            ||
            cs::endsWith(coverImageFilePath.data(), cs::MAX_SIZE_T, u".jpeg", cs::MAX_SIZE_T, true);
        const bool is_png =
            cs::endsWith(coverImageFilePath.data(), cs::MAX_SIZE_T,  u".png", cs::MAX_SIZE_T, true);

        MP4TagArtwork artwork;
        artwork.type = MP4_ART_UNDEFINED;
        if(        is_jpeg ) {
          artwork.type = MP4_ART_JPEG;
        } else if( is_png ) {
          artwork.type = MP4_ART_PNG;
        }
        if( artwork.type != MP4_ART_UNDEFINED ) {
          artwork.data = const_cast<uint8_t*>(fileData.data());
          artwork.size = static_cast<uint32_t>(fileData.size());
          MP4TagsAddArtwork(tags, &artwork);
        }
      }
    }
  } // End Conversion

  MP4FileHandle file = MP4Modify(csUnicodeToUtf8(filename).data());
  if( file == MP4_INVALID_FILE_HANDLE ) {
    MP4TagsFree(tags);
    return false;
  }
  const bool result = MP4TagsStore(tags, file);
  MP4Close(file);

  MP4TagsFree(tags);

  return result;
}

Mp4Tag Mp4Tag::read(const std::string& filename_utf8)
{
  MP4FileHandle file = MP4Read(filename_utf8.data());
  if( file == MP4_INVALID_FILE_HANDLE ) {
    return Mp4Tag();
  }

  Mp4Tag result;
  result.filename = csUtf8ToUnicode(filename_utf8);

  const MP4Tags *tags = MP4TagsAlloc();
  if( tags != nullptr  &&  MP4TagsFetch(tags, file) ) {
#define INPUT(meta,str)  if( tags->meta != nullptr ) { result.str = csUtf8ToUnicode(tags->meta); }
    INPUT(album,title);
    INPUT(name,chapter);
    INPUT(artist,author);
    INPUT(albumArtist,albumArtist);
    INPUT(composer,composer);
    INPUT(genre,genre);
#undef INPUT
    if( tags->track != nullptr ) {
      result.trackIndex = tags->track->index;
      result.trackTotal = tags->track->total;
    }
    if( tags->disk != nullptr ) {
      result.diskIndex = tags->disk->index;
      result.diskTotal = tags->disk->total;
    }
  }
  MP4TagsFree(tags);

  MP4Close(file);

  return result;
}
