#include <cstdio>
#include <cstdlib>

#include <limits>

#include <mp4v2/mp4v2.h>

int main(int argc, char **argv)
{
  MP4FileHandle file = MP4Read(argv[1]);
  if( file == MP4_INVALID_FILE_HANDLE ) {
    return EXIT_FAILURE;
  }

  MP4Dump(file, false);

  char *info = MP4Info(file, MP4_INVALID_TRACK_ID);
  if( info != nullptr ) {
    printf("\n*** MP4Info() *******************************************\n\n");
    printf("%s\n", info);
    std::free(info);
  }

  if( argc - 1 > 1 ) {
    const uint32_t trackId = static_cast<uint32_t>(argv[2][0]) - 0x30;

    printf("\n\n");

    const uint8_t id =  MP4GetTrackEsdsObjectTypeId(file, trackId);
    printf("object type id = 0x%X\n", id);

    const int              channels = MP4GetTrackAudioChannels(file, trackId);
    const MP4Duration      duration = MP4GetTrackDuration(file, trackId);
    const uint32_t        timeScale = MP4GetTrackTimeScale(file, trackId);
    const MP4SampleId    numSamples = MP4GetTrackNumberOfSamples(file, trackId);
    const MP4Duration fixedDuration = MP4GetTrackFixedSampleDuration(file, trackId);

    printf("duration = %llu\ntime scale = %d\nchannels = %d\nfixed duration = %llu\n",
           duration, timeScale, channels, fixedDuration);
    printf("duration = %.3fs\n", static_cast<double>(duration)/timeScale);
    printf("samples = %d\n", numSamples);

    MP4Duration cntDuration = 0;
    MP4Duration minDuration = std::numeric_limits<MP4Duration>::max();
    MP4Duration maxDuration = std::numeric_limits<MP4Duration>::lowest();
    for(MP4SampleId id = 1; id <= numSamples; id++) {
      const MP4Duration dur = MP4GetSampleDuration(file, trackId, id);
      cntDuration += dur;
      if( dur < minDuration ) {
        minDuration = dur;
      }
      if( dur > maxDuration ) {
        maxDuration = dur;
      }
    }
    printf("counted duration = %llu, min = %llu, max = %llu\n",
           cntDuration, minDuration, maxDuration);
  }

  MP4Close(file);

  return EXIT_SUCCESS;
}
