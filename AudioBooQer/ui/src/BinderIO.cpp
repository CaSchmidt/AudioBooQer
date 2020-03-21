/****************************************************************************
** Copyright (c) 2020, Carsten Schmidt. All rights reserved.
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

#include <QtCore/QFile>
#include <QtCore/QXmlStreamWriter>

#include "BinderIO.h"

bool saveBinder(const QString& filename, const BookBinder& binder)
{
  QFile file(filename);
  if( !file.open(QIODevice::WriteOnly | QIODevice::Truncate) ) {
    return false;
  }

  QXmlStreamWriter xml(&file);
  xml.setAutoFormatting(true);
  xml.setAutoFormattingIndent(2);
  xml.writeStartDocument();

  xml.writeStartElement(QStringLiteral("binder")); // Begin Binder ///////////

  for(const BookBinderChapter& chapter : binder) {
    xml.writeStartElement(QStringLiteral("chapter")); // Begin Chapter ///////

    xml.writeTextElement(QStringLiteral("title"), QString::fromStdU16String(chapter.first));
    xml.writeTextElement(QStringLiteral("file"), QString::fromUtf8(chapter.second.data()));

    xml.writeEndElement(); // End Chapter ////////////////////////////////////
  }

  xml.writeEndElement(); // End Binder ///////////////////////////////////////

  xml.writeEndDocument();

  return true;
}
