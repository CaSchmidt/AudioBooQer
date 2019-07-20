#include <cstdio>
#include <cstdlib>

#include <QtCore/QFile>

#include "adts.h"

int main(int /*argc*/, char **argv)
{
  AdtsParser::Buffer buf;

  QFile file(QString::fromLatin1(argv[1]));
  if( !file.open(QIODevice::ReadOnly) ) {
    return EXIT_FAILURE;
  }

  buf.resize(static_cast<AdtsParser::size_type>(file.size()), 0);
  file.seek(0);
  file.read(reinterpret_cast<char*>(buf.data()), static_cast<qint64>(buf.size()));

  file.close();

  AdtsParser p(std::move(buf));

  uint32_t i = 0;
  while( p.hasFrame() ) {
    printf("%6d: #%llu, %4llubytes, MPEG-%d\n",
           i, p.frameCount(), p.frameSize(), p.isMpeg2Frame() ? 2 : 4);

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
