/****************************************************************************
** Copyright (c) 2014, Carsten Schmidt. All rights reserved.
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

#ifndef JOB_H
#define JOB_H

#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QStringList>

#include "AacFormat.h"

class IAudioEncoder;

struct Job {
  Job() = default;

  QString outputFilePath(IAudioEncoder *encoder) const;

  AacFormat format{};
  QStringList inputFiles{};
  QString outputDirPath{};
  int position{};
  bool renameInput{false};
  QString title{};
};

using Jobs = QList<Job>;

struct JobResult {
  JobResult() = default;

  QString message{};
  uint64_t numTimeSamples{};
  QString outputFilePath{};
  int position{};
  QString title{};

  constexpr bool isValid() const
  {
    return numTimeSamples > 0;
  }

  constexpr bool operator<(const JobResult& other) const
  {
    return position < other.position;
  }
};

using JobResults = QList<JobResult>;

JobResult executeJob(const Job& job);

#endif // JOB_H
