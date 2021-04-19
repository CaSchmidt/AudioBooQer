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
#include <QtCore/QXmlStreamReader>
#include <QtCore/QXmlStreamWriter>

#include <csUtil/csQStringUtil.h>

#include "BinderIO.h"

#define XML_BINDER   QStringLiteral("binder")
#define XML_CHAPTER  QStringLiteral("chapter")
#define XML_FILE     QStringLiteral("file")
#define XML_TITLE    QStringLiteral("title")

namespace priv {

  void readChapter(QXmlStreamReader& xml, BookBinder& binder)
  {
    QString file;
    QString title;
    while( xml.readNextStartElement() ) {
      if(        xml.name() == XML_TITLE ) {
        title = xml.readElementText();
      } else if( xml.name() == XML_FILE ) {
        file  = xml.readElementText();
      } else {
        xml.skipCurrentElement();
      }
    }

    if( file.isEmpty()  ||  title.isEmpty() ) {
      return;
    }

    BookBinderChapter chapter;
    chapter.first  = cs::toUtf16String(title);
    chapter.second = cs::toUtf8String(file);
    binder.push_back(chapter);
  }

  void readBinder(QXmlStreamReader& xml, BookBinder& binder)
  {
    while( xml.readNextStartElement() ) {
      if( xml.name() == XML_CHAPTER ) {
        readChapter(xml, binder);
      } else {
        xml.skipCurrentElement();
      }
    }
  }

} // namespace priv

BookBinder openBinder(const QString& filename)
{
  BookBinder result;

  QFile file(filename);
  if( !file.open(QIODevice::ReadOnly) ) {
    return BookBinder();
  }

  QXmlStreamReader xml(&file);

  while( xml.readNextStartElement() ) {
    if( xml.name() == XML_BINDER ) {
      priv::readBinder(xml, result);
    } else {
      xml.skipCurrentElement();
    }
  }

  return result;
}

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

  xml.writeStartElement(XML_BINDER); // Begin Binder /////////////////////////

  for(const BookBinderChapter& chapter : binder) {
    xml.writeStartElement(XML_CHAPTER); // Begin Chapter /////////////////////

    xml.writeTextElement(XML_TITLE, cs::toQString(chapter.first));
    xml.writeTextElement(XML_FILE,  cs::toQString(chapter.second));

    xml.writeEndElement(); // End Chapter ////////////////////////////////////
  }

  xml.writeEndElement(); // End Binder ///////////////////////////////////////

  xml.writeEndDocument();

  return true;
}
