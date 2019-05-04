#include <cstdio>
#include <cstdlib>

#include <cstdint>

#include <vector>

#include <mp4v2/mp4v2.h>

class File {
public:
  File() noexcept = default;

  ~File() noexcept
  {
    close();
  }

  File(File&&) noexcept = default;
  File& operator=(File&&) noexcept = default;

  bool isOpen() const
  {
    return _file != nullptr;
  }

  bool open(const char *filename, const bool is_write = false)
  {
    _file = std::fopen(filename, is_write
                       ? "wb"
                       : "rb");
    return isOpen();
  }

  void close()
  {
    if( _file != nullptr ) {
      std::fclose(_file);
      _file = nullptr;
    }
  }

  std::size_t pos() const
  {
    return static_cast<std::size_t>(std::ftell(_file));
  }

  bool seek(const std::size_t pos) const
  {
    return std::fseek(_file, static_cast<long>(pos), SEEK_SET) == 0;
  }

  std::size_t size() const
  {
    const std::size_t cur = pos();
    if( std::fseek(_file, 0, SEEK_END) != 0 ) {
      return 0;
    }
    const std::size_t siz = pos();
    if( !seek(cur) ) {
      return 0;
    }
    return siz;
  }

  std::vector<uint8_t> readAll() const
  {
    std::vector<uint8_t> result;
    try {
      result.resize(size(), 0);
    } catch(...) {
      result.clear();
    }
    if( !read(result.data(), result.size()) ) {
      result.clear();
    }
    // printf("fread = %zu\n", std::fread(result.data(), 1, result.size(), _file));
    /*
    printf("result = %llu\n", result.size());
    if( !result.empty()  &&
        std::fread(result.data(), result.size(), 1, _file) != 1 ) {
      printf("hier\n");
      result.clear();
    }
    */
    return result;
  }

private:
  File(const File&) noexcept = delete;
  File& operator=(const File&) noexcept = delete;

  bool read(uint8_t *buffer, const std::size_t size, const std::size_t blockSize = 1024) const
  {
    const std::size_t numBlocks = size/blockSize;
    const std::size_t remBlocks = size%blockSize;

    for(std::size_t i = 0; i < numBlocks; i++) {
      const std::size_t got = std::fread(buffer, 1, blockSize, _file);
      if( got != blockSize ) {
        return false;
      }
      buffer += blockSize;
    }

    if( remBlocks > 0 ) {
      const std::size_t got = std::fread(buffer, 1, remBlocks, _file);
      if( got != remBlocks ) {
        return false;
      }
    }

    return true;
  }

  FILE *_file{nullptr};
};

int main(int /*argc*/, char **argv)
{
  File input;
  if( !input.open(argv[1]) ) {
    return EXIT_FAILURE;
  }
  printf("input = %llu\n", input.size());
  const std::vector<uint8_t> data = input.readAll();
  printf("data = %llu\n", data.size());

  const MP4FileHandle output = MP4CreateEx("output.m4a", 0, 1, 0, "M4A ", 0);

  const MP4TrackId trackId = MP4AddAudioTrack(output, 44100, MP4_INVALID_DURATION, MP4_MPEG4_AUDIO_TYPE);
  MP4WriteSample(output, trackId, data.data(), data.size());

  MP4Close(output);

  return EXIT_SUCCESS;
}
