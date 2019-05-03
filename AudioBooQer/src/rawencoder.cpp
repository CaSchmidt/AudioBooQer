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

#include <QtCore/QDir>

#include "rawencoder.h"

////// public ////////////////////////////////////////////////////////////////

RawEncoder::RawEncoder()
  : _file()
{
}

RawEncoder::~RawEncoder()
{
  _file.close();
}

bool RawEncoder::encode(const QAudioBuffer& buffer)
{
  if( !buffer.isValid() ) {
    return false;
  }
  const char *data = reinterpret_cast<const char *>(buffer.data());
  return _file.write(data, buffer.byteCount()) == buffer.byteCount();
}

bool RawEncoder::initialize(const QAudioFormat& format,
                            const QString& outputDirPath,
                            const QString& nameHint)
{
  if( _file.isOpen()  ||  format.channelCount() > 2 ) {
    return false;
  }

  QString filePath;
  {
    QString fileName(nameHint);
    fileName.append(format.byteOrder() == QAudioFormat::BigEndian
                    ? QStringLiteral(".be")
                    : QStringLiteral(".le"));
    fileName.append(QStringLiteral(".%1ch").arg(format.channelCount()));
    fileName.append(QStringLiteral(".%1%2")
                    .arg(format.sampleType() == QAudioFormat::UnSignedInt
                         ? QStringLiteral("u")
                         : QStringLiteral("s"))
                    .arg(format.sampleSize()));
    fileName.append(QStringLiteral(".%1Hz").arg(format.sampleRate()));
    fileName.append(QStringLiteral(".raw"));

    filePath = QDir(outputDirPath).absoluteFilePath(fileName);
  }

  _file.setFileName(filePath);
  return _file.open(QIODevice::WriteOnly);
}

QString RawEncoder::outputFilename() const
{
  return _file.fileName();
}
