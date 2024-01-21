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

#include <aacenc_lib.h>

#include <cs/IO/File.h>

#include "AacEncoder.h"

#include "Mpeg4Audio.h"

////// Implementation ////////////////////////////////////////////////////////

inline constexpr int numBytesPerSample = 2;

static_assert(numBytesPerSample == sizeof(INT_PCM));

static_assert(mpeg4::numSamplesPerAacFrame == 1024);

/*
 * NOTE:
 * - FDK AAC seems to operate on native endian, signed 16bit integers ONLY!
 *   (cf. INT_PCM, libSYS/include/machine_type.h)
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

  bool setParam(const AACENC_PARAM param, const UINT value);

  uint8_t           bitstream[64*1024];
  cs::File          file;
  AacFormat         format{};
  HANDLE_AACENCODER handle{};
  BufferDesc        inDesc{};
  AACENC_InfoStruct info{};
  uint64_t          numDataSamples{};
  BufferDesc        outDesc{};
  uint8_t           zeros[64*1024];
};

bool AacEncoderImpl::setParam(const AACENC_PARAM param, const UINT value)
{
  return aacEncoder_SetParam(handle, param, value) == AACENC_OK;
}

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

bool AacEncoder::encode(const void *data, const std::size_t size)
{
  if( !isValidData(data, size) ) {
    return false;
  }
  return encodeBlock(reinterpret_cast<const uint8_t*>(data), int(size));
}

bool AacEncoder::flush()
{
  // (1) Compute number of samples to fill ///////////////////////////////////

  const unsigned int frameLength = mpeg4::numSamplesPerAacFrame;
  const unsigned int  mod = static_cast<unsigned int>(numPcmFrames())%frameLength;
  const unsigned int fill = mod > 0
      ? frameLength - mod
      : 0;

  // (2) Fill remaining frame with zeros /////////////////////////////////////

  if( 0 < fill  &&  fill < mpeg4::numSamplesPerAacFrame  &&
      !encodeBlock(impl->zeros, int(fill*impl->info.inputChannels)*numBytesPerSample) ) {
    return false;
  }

  // (3) Flush data to disk //////////////////////////////////////////////////

  bool eof = false;
  while( !eof ) {
    if( !encodeBlock(impl->zeros, 0, &eof) ) {
      return false;
    }
  }
  return true;
}

bool AacEncoder::initialize(const AacFormat& format, const std::filesystem::path& outputFileName)
{
  // (0) Sanity check ////////////////////////////////////////////////////////

  CHANNEL_MODE mode = MODE_INVALID;
  if(        format.numChannels == 1 ) {
    mode = MODE_1;
  } else if( format.numChannels == 2 ) {
    mode = MODE_2;
  }

  if( impl                 ||  // do not operate on an existing instance
      !format.isValid()    ||  // proper audio format passed in
      mode == MODE_INVALID ) { // support Mono/Stereo only
    return false;
  }

  std::unique_ptr<AacEncoderImpl> result = std::make_unique<AacEncoderImpl>();
  result->format = format;

  // (1) Open encoder ////////////////////////////////////////////////////////

  if( aacEncOpen(&result->handle, 0, UINT(format.numChannels)) != AACENC_OK ) {
    return false;
  }

  if( !result->setParam(AACENC_AOT, AOT_AAC_LC) ) {
    return false;
  }

  if( !result->setParam(AACENC_BITRATE, 64000) ) {
    return false;
  }

  if( !result->setParam(AACENC_BITRATEMODE, 0) ) {
    return false;
  }

  if( !result->setParam(AACENC_SAMPLERATE, UINT(format.numSamplesPerSecond)) ) {
    return false;
  }

  if( !result->setParam(AACENC_CHANNELMODE, UINT(mode)) ) {
    return false;
  }

  if( !result->setParam(AACENC_CHANNELORDER, 1) ) {
    return false;
  }

  if( !result->setParam(AACENC_AFTERBURNER, 1) ) {
    return false;
  }

  if( !result->setParam(AACENC_GRANULE_LENGTH, UINT(mpeg4::numSamplesPerAacFrame)) ) {
    return false;
  }

  if( !result->setParam(AACENC_TRANSMUX, TT_MP4_ADTS) ) {
    return false;
  }

  if( !result->setParam(AACENC_SIGNALING_MODE, 0) ) {
    return false;
  }

  if( !result->setParam(AACENC_HEADER_PERIOD, 0) ) {
    return false;
  }

  if( !result->setParam(AACENC_TPSUBFRAMES, 1) ) {
    return false;
  }

  if( aacEncEncode(result->handle, nullptr, nullptr, nullptr, nullptr) != AACENC_OK ) {
    return false;
  }

  if( aacEncInfo(result->handle, &result->info) != AACENC_OK ) {
    return false;
  }

  // (2) Create output file //////////////////////////////////////////////////

  if( !result->file.open(outputFileName, cs::FileOpenFlag::Write) ) {
    return false;
  }

  // (3) Store result ////////////////////////////////////////////////////////

  impl = std::move(result);
  impl->inDesc.initialize(IN_AUDIO_DATA, numBytesPerSample);
  impl->outDesc.initialize(OUT_BITSTREAM_DATA, 1);
  std::memset(impl->zeros, 0, sizeof(impl->zeros));

  return true;
}

uint64_t AacEncoder::numPcmFrames() const
{
  return impl->numDataSamples/impl->info.inputChannels;
}

std::filesystem::path AacEncoder::outputSuffix(const AacFormat&) const
{
  return "aac";
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
        : size/numBytesPerSample;

    AACENC_OutArgs out_args;
    const AACENC_ERROR error =
        aacEncEncode(impl->handle, impl->inDesc, impl->outDesc, &in_args, &out_args);

    if( eof != nullptr ) {
      *eof = error == AACENC_ENCODE_EOF;
    }

    if( error != AACENC_OK  &&  error != AACENC_ENCODE_EOF ) {
      return false;
    }

    impl->numDataSamples += uint64_t(out_args.numInSamples);

    if( out_args.numOutBytes > 0  &&
        impl->file.write(impl->bitstream, out_args.numOutBytes) != std::size_t(out_args.numOutBytes) ) {
      return false;
    }

    if( in_args.numInSamples - out_args.numInSamples > 0 ) {
      data += out_args.numInSamples*numBytesPerSample;
      size -= out_args.numInSamples*numBytesPerSample;
    } else {
      break;
    }
  }

  return true;
}
