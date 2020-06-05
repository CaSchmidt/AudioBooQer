#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>

#include <mp4v2/mp4v2.h>

#define ARG_COUNT  argv[1]
#define ARG_BASE   argv[2]
#define ARG_FILE   argv[3]

bool dump(const char *base, const uint32_t id, const uint32_t numBytes, const uint8_t *bytes)
{
  if( numBytes < 1  ||  bytes == nullptr ) {
    return false;
  }

  char filename[128];
  sprintf(filename, "%s%04d.bin", base, id);

  FILE *file = fopen(filename, "wb");
  if( file == NULL ) {
    return false;
  }

  fwrite(bytes, numBytes, 1, file);

  fclose(file);

  return true;
}

void dumpHexRow(const uint8_t *bytes, const uint32_t offset, const uint32_t numBytes)
{
  if( numBytes < 1  ||  numBytes > 16 ) {
    return;
  }
  printf("%08X:", offset);
  for(uint32_t byte = 0; byte < numBytes; byte++) {
    printf(" %02X", bytes[offset + byte]);
  }
  printf("\n");
}

void dumpHex(const uint32_t numBytes, const uint8_t *bytes)
{
  const uint32_t numBlocks = numBytes/16;
  const uint32_t remBytes  = numBytes%16;

  for(uint32_t block = 0; block < numBlocks; block++) {
    const uint32_t offset = block*16;
    dumpHexRow(bytes, offset, 16);
  }

  dumpHexRow(bytes, numBlocks*16, remBytes);
}

int main(int argc, char **argv)
{
  if( argc - 1 < 3 ) {
    fprintf(stderr, "Usage: %s <count> <base> <file>\n", argv[0]);
    return EXIT_FAILURE;
  }

  const MP4SampleId count = strtoul(ARG_COUNT, NULL, 10);

  MP4FileHandle file = MP4Read(ARG_FILE);
  if( file == MP4_INVALID_FILE_HANDLE ) {
    fprintf(stderr, "ERROR: Unable to open file \"%s\"!\n", ARG_FILE);
    return EXIT_FAILURE;
  }

  MP4TrackId trackId = 0;
  for(MP4TrackId id = 1; id <= MP4GetNumberOfTracks(file); id++) {
    if( strcmp(MP4GetTrackType(file, id), "soun") == 0 ) {
      trackId = id;
      break;
    }
  }

  if( trackId == 0 ) {
    MP4Close(file);
    return EXIT_FAILURE;
  }

  for(MP4SampleId sampleId = 1; sampleId <= count; sampleId++) {
    uint8_t *bytes = nullptr;
    uint32_t numBytes = 0;
    if( !MP4ReadSample(file, trackId, sampleId, &bytes, &numBytes) ) {
      fprintf(stderr, "ERROR: Unable to read sample %d of track %d!\n", sampleId, trackId);
      break;
    }
    if( strcmp(ARG_BASE, "-hex") == 0 ) {
      dumpHex(numBytes, bytes);
      printf("---\n");
    } else {
      dump(ARG_BASE, sampleId, numBytes, bytes);
    }
    MP4Free(bytes);
  }

  MP4Close(file);

  return EXIT_SUCCESS;
}
