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

#include <mp4v2/mp4v2.h>

#include "mp4tag.h"

////// Public ////////////////////////////////////////////////////////////////

bool Mp4Tag::isValid() const
{
  return !filename.isEmpty();
}

Mp4Tag readTag(const QString& filename)
{
  MP4FileHandle file = MP4Read(filename.toUtf8().constData());
  if( file == MP4_INVALID_FILE_HANDLE ) {
    return Mp4Tag();
  }

  Mp4Tag result;
  result.filename = filename;

  const MP4Tags *tags = MP4TagsAlloc();
  if( tags != nullptr  &&  MP4TagsFetch(tags, file) ) {
    if( tags->album != nullptr ) {
      result.title = QString::fromUtf8(tags->album);
    }
    if( tags->name != nullptr ) {
      result.chapter = QString::fromUtf8(tags->name);
    }
    if( tags->artist != nullptr ) {
      result.author = QString::fromUtf8(tags->artist);
    }
    if( tags->albumArtist != nullptr ) {
      result.albumArtist = QString::fromUtf8(tags->albumArtist);
    }
    if( tags->composer != nullptr ) {
      result.composer = QString::fromUtf8(tags->composer);
    }
    if( tags->genre != nullptr ) {
      result.genre = QString::fromUtf8(tags->genre);
    }
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
