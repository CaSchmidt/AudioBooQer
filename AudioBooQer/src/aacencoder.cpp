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

#include <cstdint>
#include <cstring>

#include <vector>

#include <QtCore/QDir>
#include <QtCore/QFile>

#include <aacenc_lib.h>

#include "aacencoder.h"

////// Implementation ////////////////////////////////////////////////////////

/*
 * NOTE:
 * - FDK AAC seems to operate on native endian, signed 16bit integers ONLY!
 * - We support only Mono & Stereo.
 * - The bitrate is fixed to 64k.
 */

class BufferDesc {
public:
  BufferDesc() = default;
  ~BufferDesc() = default;

  void assign(const void *buffer, const int size);
  void initialize(const int id, const int el_size);

  operator AACENC_BufDesc*();

private:
  void          *_buf{};
  int            _bufferIdentifier{};
  int            _bufSize{};
  int            _bufElSize{};
  AACENC_BufDesc _desc{};
};

void BufferDesc::assign(const void *buffer, const int size)
{
  _buf     = const_cast<void*>(buffer);
  _bufSize = size;
}

void BufferDesc::initialize(const int id, const int el_size)
{
  _buf              = nullptr;
  _bufferIdentifier = id;
  _bufSize          = 0;
  _bufElSize        = el_size;

  _desc.numBufs           = 1;
  _desc.bufs              = &_buf;
  _desc.bufferIdentifiers = &_bufferIdentifier;
  _desc.bufSizes          = &_bufSize;
  _desc.bufElSizes        = &_bufElSize;
}

BufferDesc::operator AACENC_BufDesc*()
{
  return &_desc;
}

class AacEncoderImpl {
public:
  AacEncoderImpl() = default;
  ~AacEncoderImpl() = default;

  uint8_t           bitstream[64*1024];
  QFile             file{};
  HANDLE_AACENCODER handle{};
  BufferDesc        inDesc{};
  BufferDesc        outDesc{};
  uint8_t           zeros[64*1024];
};

////// public ////////////////////////////////////////////////////////////////

AacEncoder::AacEncoder()
  : impl()
{
}

AacEncoder::~AacEncoder()
{
  if( impl ) {
    impl->file.close();
    aacEncClose(&impl->handle);
  }
}

bool AacEncoder::isNull() const
{
  return !impl;
}

bool AacEncoder::encode(const QAudioBuffer& buffer)
{
  if( !buffer.isValid() ) {
    return false;
  }
  return encodeBlock(reinterpret_cast<const uint8_t*>(buffer.data()), buffer.byteCount());
}

bool AacEncoder::flush()
{
  bool eof = false;
  while( !eof ) {
    if( !encodeBlock(impl->zeros, 0, &eof) ) {
      return false;
    }
  }
  return true;
}

bool AacEncoder::initialize(const QAudioFormat& format,
                            const QString& outputDirPath,
                            const QString& title)
{
  // (0) Sanity check ////////////////////////////////////////////////////////

  CHANNEL_MODE mode = MODE_INVALID;
  if(        format.channelCount() == 1 ) {
    mode = MODE_1;
  } else if( format.channelCount() == 2 ) {
    mode = MODE_2;
  }

  if( impl  ||  format.sampleSize() != 16  ||  mode == MODE_INVALID ) {
    return false;
  }

  std::unique_ptr<AacEncoderImpl> result = std::make_unique<AacEncoderImpl>();

  // (1) Open encoder ////////////////////////////////////////////////////////

  if( aacEncOpen(&result->handle, 0, static_cast<UINT>(format.channelCount())) != AACENC_OK ) {
    return false;
  }

  if( aacEncoder_SetParam(result->handle, AACENC_AOT, AOT_AAC_LC) != AACENC_OK ) {
    return false;
  }

  if( aacEncoder_SetParam(result->handle, AACENC_SAMPLERATE, static_cast<UINT>(format.sampleRate())) != AACENC_OK ) {
    return false;
  }

  if( aacEncoder_SetParam(result->handle, AACENC_CHANNELMODE, static_cast<UINT>(mode)) != AACENC_OK ) {
    return false;
  }

  if( aacEncoder_SetParam(result->handle, AACENC_CHANNELORDER, 1) != AACENC_OK ) {
    return false;
  }

  if( aacEncoder_SetParam(result->handle, AACENC_BITRATE, 64000) != AACENC_OK ) {
    return false;
  }

  if( aacEncoder_SetParam(result->handle, AACENC_TRANSMUX, TT_MP4_ADTS) != AACENC_OK ) {
    return false;
  }

  if( aacEncoder_SetParam(result->handle, AACENC_AFTERBURNER, 1) != AACENC_OK ) {
    return false;
  }

  if( aacEncEncode(result->handle, nullptr, nullptr, nullptr, nullptr) != AACENC_OK ) {
    return false;
  }

  // (2) Create output file //////////////////////////////////////////////////

  result->file.setFileName(QDir(outputDirPath).absoluteFilePath(QStringLiteral("%1.aac").arg(title)));
  if( !result->file.open(QIODevice::WriteOnly) ) {
    return false;
  }

  // (3) Store result ////////////////////////////////////////////////////////

  impl = std::move(result);
  impl->inDesc.initialize(IN_AUDIO_DATA, 2);
  impl->outDesc.initialize(OUT_BITSTREAM_DATA, 1);
  std::memset(impl->zeros, 0, sizeof(impl->zeros));

  return true;
}

QString AacEncoder::outputFilename() const
{
  return impl->file.fileName();
}

////// private ///////////////////////////////////////////////////////////////

bool AacEncoder::encodeBlock(const uint8_t *data, int size, bool *eof)
{  
  while( true ) {
    impl->inDesc.assign(data, size);
    impl->outDesc.assign(impl->bitstream, sizeof(impl->bitstream));

    AACENC_InArgs in_args;
    in_args.numInSamples = size < 1
        ? -1      // Flush encoder!
        : size/2;

    AACENC_OutArgs out_args;
    const AACENC_ERROR error =
        aacEncEncode(impl->handle, impl->inDesc, impl->outDesc, &in_args, &out_args);

    if( eof != nullptr ) {
      *eof = error == AACENC_ENCODE_EOF;
    }

    if( error != AACENC_OK  &&  error != AACENC_ENCODE_EOF ) {
      return false;
    }

    if( out_args.numOutBytes > 0  &&
        impl->file.write(reinterpret_cast<const char*>(impl->bitstream), out_args.numOutBytes) != out_args.numOutBytes ) {
      return false;
    }

    if( in_args.numInSamples - out_args.numInSamples > 0 ) {
      data += out_args.numInSamples*2;
      size -= out_args.numInSamples*2;
    } else {
      break;
    }
  }

  return true;
}
