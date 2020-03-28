# AudioBooQer

## Introduction

**AudioBooQer** is an application that lets you create audiobooks from separate audio files.

The resulting audiobook will be an `AAC` encoded, **iTunes** compatible `M4B` (`MP4`) file.

## Quick Start

### Step 1: Create chapters

Open a directory (`Ctrl+O`) with the tracks of your audiobook and group them into chapters.

![Step 1](AudioBooQer/docs/QuickStart/step1.png)

While encoding the chapters to `AAC`, **AudioBooQer** uses all available cores!

![Step 1 - Encoding](AudioBooQer/docs/QuickStart/step1_encoding.png)

### Step 2: Bind an audiobook

Bind the encoded chapters (`Ctrl+B`) to a single audiobook in `M4B` format.

![Step 2](AudioBooQer/docs/QuickStart/step2.png)

### Step 3: Tag the audiobook

Tag the audiobook (`Ctrl+T`) to supply meta information.

![Step 3](AudioBooQer/docs/QuickStart/step3.png)

## Internals AKA How is it done?

You may also want to take a look at the [References](AudioBooQer/docs/References.md).

The main goal while developing **AudioBooQer** was to create audiobooks that are playable with **iTunes**
with support for all its bells and whistles like chapter names, seeking and bookmarks.

As it turns out, audiobooks for the **iTunes** store, usually distributed as `M4B` files,
are just `AAC` encoded audio streams with a separate text stream providing the chapter marks.
The infamous `M4B` format used for distribution is a derivative of the `MP4` format
specified in the MPEG-4 / ISO 14996 standard.

However, encoding audio to `AAC`, writing the stream to `MP4` and
renaming the file to `MP4` will not produce a valid `M4B` audiobook recognized by **iTunes**.
For an `MP4` file to be recognized by **iTunes** as an audiobook the following constraints must be met:

- The audio stream is stored in fixed time intervals or "samples" according to the MPEG-4 standard.
  Usually the `AAC` frame length is used.
  The conversion to time units is performed using the "time scale" which resembles the sampling rate of the audio stream.
- A separate text track is written to provide the chapter boundaries.

Creating audiobooks is performed in two steps:

1. Encoding of audio streams for each chapter using the [FDK AAC](https://github.com/mstorsjo/fdk-aac) library.
  - For each chapter, the selected audio files are encoded to a consecutive audio stream.
  - Decoding the individual audio files is performed using [QAudioDecoder](https://doc.qt.io/qt-5/qaudiodecoder.html).
  - The class `QAudioDecoder` closely interacts with the class [AudioJob](AudioBooQer/ui/include/AudioJob.h)
    to produced a consecutive audio stream.
  - Encoding to `AAC` audio is provided by the class [AacEncoder](AudioBooQer/audiobook/include/AacEncoder.h) which
    implements the [IAudioEncoder](AudioBooQer/audiobook/include/IAudioEncoder.h) interface.
  - The resulting `AAC` stream is written to an `ADTS` stream.
  - Upon finishing each file set, the `AAC` stream is padded to complete `AAC` frames by inserting zero samples.
