#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <limits>

#include <mp4v2/mp4v2.h>

void print_tags(const MP4FileHandle file)
{
  const MP4Tags *tags = MP4TagsAlloc();
  if( MP4TagsFetch(tags, file) ) {
    printf("name         = %s\n", tags->name);
    printf("artist       = %s\n", tags->artist);
    printf("album artist = %s\n", tags->albumArtist);
    printf("album        = %s\n", tags->album);
    printf("grouping     = %s\n", tags->grouping);
    printf("composer     = %s\n", tags->composer);
    printf("comments     = %s\n", tags->comments);
    printf("genre        = %s\n", tags->genre);
    printf("release date = %s\n", tags->releaseDate);
    if( tags->track != nullptr ) {
      printf("track        = %d/%d\n", tags->track->index, tags->track->total);
    }
    if( tags->disk != nullptr ) {
      printf("disk         = %d/%d\n", tags->disk->index, tags->disk->total);
    }
    printf("description  = %s\n", tags->description);
    printf("long descr.  = %s\n", tags->longDescription);
    printf("lyrics       = %s\n", tags->lyrics);
    printf("--- sorting ---\n");
    printf("name         = %s\n", tags->sortName);
    printf("artist       = %s\n", tags->sortArtist);
    printf("album artist = %s\n", tags->sortAlbumArtist);
    printf("album        = %s\n", tags->sortAlbum);
    printf("composer     = %s\n", tags->sortComposer);
    printf("copyright    = %s\n", tags->copyright);
    printf("encoding tl. = %s\n", tags->encodingTool);
    printf("encoded by   = %s\n", tags->encodedBy);
    printf("purchase dt. = %s\n", tags->purchaseDate);
    printf("keywords     = %s\n", tags->keywords);
    printf("category     = %s\n", tags->category);
    printf("iTunes acc.  = %s\n", tags->iTunesAccount);
    printf("xid          = %s\n", tags->xid);
  }
  MP4TagsFree(tags); tags = nullptr;
}

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
    const MP4TrackId trackId = static_cast<MP4TrackId>(argv[2][0]) - 0x30;

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

    uint8_t *config;
    uint32_t numConfig;
    if( MP4GetTrackESConfiguration(file, trackId, &config, &numConfig)  &&
        config != nullptr ) {
      printf("config:");
      for(uint32_t i = 0; i < numConfig; i++) {
        printf(" %02X", config[i]);
      }
      printf("\n");
      MP4Free(config);
    }

    uint64_t flags = 0;
    if( MP4GetTrackIntegerProperty(file, trackId, "tkhd.flags", &flags) ) {
      std::cout << "flags: " << std::showbase << std::hex << flags << std::endl;
    }

    char language[4] = { 0, 0, 0, 0 };
    MP4GetTrackLanguage(file, trackId, language);
    printf("language: %s\n", language);
  }

  MP4Close(file);

  return EXIT_SUCCESS;
}
