#include <cstdio>
#include <cstdlib>
#include <cstdint>

#include <algorithm>
#include <string>

#include <mp4v2/mp4v2.h>

#define ARG_FLAGS  argv[1]
#define ARG_LANG   argv[2]
#define ARG_FILE   argv[3]

template<typename CharT>
constexpr bool compare(const CharT *s1, const CharT *s2)
{
  using Traits = std::char_traits<CharT>;
  return Traits::compare(s1, s2, std::min(Traits::length(s1), Traits::length(s2))) == 0;
}

template<typename CharT>
constexpr std::size_t length(const CharT *s)
{
  using Traits = std::char_traits<CharT>;
  return Traits::length(s);
}

int main(int argc, char **argv)
{
  if( argc - 1 < 3 ) {
    fprintf(stderr, "Usage: %s <flags> <lang> <file>\n", argv[0]);
    return EXIT_FAILURE;
  }

  int64_t flags = strtoll(ARG_FLAGS, NULL, 16) & 0xF; // TODO: C++17, check range

  if( length(ARG_LANG) != 3 ) { // TODO: check for lower case letters
    fprintf(stderr, "ERROR: Invalid language code!\n");
    return EXIT_FAILURE;
  }

  MP4FileHandle file = MP4Modify(ARG_FILE);
  if( file == MP4_INVALID_FILE_HANDLE ) {
    fprintf(stderr, "ERROR: Unable to open file \"%s\"!\n", ARG_FILE);
    return EXIT_FAILURE;
  }

  const uint32_t numTracks = MP4GetNumberOfTracks(file);
  for(MP4TrackId trackId = 1; trackId <= numTracks; trackId++) {
    if( !MP4SetTrackIntegerProperty(file, trackId, "tkhd.flags", flags) ) {
      fprintf(stderr, "ERROR: Unable to set flags for track #%d!\n", trackId);
    }

    if( compare(MP4GetTrackType(file, trackId), "text") ) {
      if( !MP4SetTrackLanguage(file, trackId, ARG_LANG) ) {
        fprintf(stderr, "ERROR: Unable to set language for track #%d!\n", trackId);
      }
    }
  }

  MP4Close(file);

  return EXIT_SUCCESS;
}
