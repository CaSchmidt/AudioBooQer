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

#include "AudioBuffer.h"

////// Constants /////////////////////////////////////////////////////////////

inline constexpr qint64 maxBufferSize = 50*1024*1024;

////// public ////////////////////////////////////////////////////////////////

AudioBuffer::AudioBuffer()
{
}

AudioBuffer::~AudioBuffer()
{
  close();
}

AudioBuffer::operator bool() const
{
  return _device != nullptr;
}

AudioBuffer::operator QIODevice*() const
{
  return _device;
}

void AudioBuffer::close()
{
  _buffer.close();
  _data.clear();
  _device = nullptr;
  _file.close();
}

bool AudioBuffer::open(const QString& filename)
{
  close();

  // (1) Open File ///////////////////////////////////////////////////////////

  _file.setFileName(filename);
  if( !_file.open(QIODevice::ReadOnly) ) {
    close();
    return false;
  }

  // (2) Buffer File if Size does not Exceed Limit ///////////////////////////

  if( _file.size() <= maxBufferSize ) {
    _data = _file.readAll();
    if( !fileIsBuffered() ) {
      _data.clear();
    }
  }

  // (4) Open Buffer if File is Buffered in Memory ///////////////////////////

  if( fileIsBuffered() ) {
    _buffer.setBuffer(&_data);
    _buffer.open(QIODevice::ReadOnly);
  }

  // (5) Initialize Device ///////////////////////////////////////////////////

  if( _buffer.isOpen() ) {
    _device = &_buffer;
  } else {
    _device = &_file;
  }

  return true;
}

QString AudioBuffer::fileName() const
{
  return _file.fileName();
}

////// private ///////////////////////////////////////////////////////////////

bool AudioBuffer::fileIsBuffered() const
{
  return static_cast<qint64>(_data.size()) == _file.size();
}
