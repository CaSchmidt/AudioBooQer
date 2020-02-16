#include <cstdio>
#include <cstdlib>

#include <csUtil/csFileIO.h>

#include "AdtsParser.h"
#include "Mpeg4Audio.h"

int main(int /*argc*/, char **argv)
{
  AdtsParser::Buffer buf = csReadBinaryFile(argv[1]);
  AdtsParser p(std::move(buf));

  uint32_t i = 0;
  while( p.hasFrame() ) {
    const uint16_t asc = p.mpeg4AudioSpecificConfig();
    printf("%6d: AAC frames=%llu, AOT=%d, Channel Configuration=%d, Frequency=%dHz\n",
           i, p.aacFrameCount(),
           mpeg4::audioObjectTypeFromASC(asc),
           mpeg4::channelConfigurationFromASC(asc),
           mpeg4::samplingFrequencyFromASC(asc));

    if( p.frameSize() < 100 ) {
      printf("data:");
      for(uint32_t j = 0; j < p.frameSize(); j++) {
        const uint8_t data = p.frameData()[j];
        printf(" %02X", data);
      }
      printf("\n");
    }

    i++;
    p.nextFrame();
  }

  return EXIT_SUCCESS;
}
