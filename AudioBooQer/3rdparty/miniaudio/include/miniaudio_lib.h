#ifndef MINIAUDIO_LIB_H
#define MINIAUDIO_LIB_H

#define MA_ENABLE_ONLY_SPECIFIC_BACKENDS
#define MA_NO_ENCODING
#define MA_NO_FLAC
#define MA_NO_DEVICE_IO
#define MA_NO_THREADING
#define MA_NO_GENERATION

#include <extras/speex_resampler/ma_speex_resampler.h>

#include <miniaudio.h>

#endif // MINIAUDIO_LIB_H
