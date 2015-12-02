// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <sys/time.h>
#include "testutil.h"

#include "random.h"


uint64_t NowMicros() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return static_cast<uint64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
}

void SleepForMicroseconds(int micros) {
    usleep(micros);
}

std::string TimeToString()
{
    char charFormat[30];

    struct timeval nowtimeval;
    gettimeofday(&nowtimeval,0);

    time_t now;
    struct tm *timenow;

    time(&now);
    timenow = localtime(&now);

    sprintf(charFormat,
        "%4.4d%2.2d%2.2d:%2.2d:%2.2d:%2.2d.%lu",
        timenow->tm_year + 1900,
        timenow->tm_mon + 1,
        timenow->tm_mday,
        timenow->tm_hour,
        timenow->tm_min,
        timenow->tm_sec, nowtimeval.tv_usec/1000);

    return charFormat;
}



Slice RandomString(Random* rnd, int len, std::string* dst) {
  dst->resize(len);
  for (int i = 0; i < len; i++) {
    (*dst)[i] = static_cast<char>(' ' + rnd->Uniform(95));   // ' ' .. '~'
  }
  return Slice(*dst);
}

std::string RandomKey(Random* rnd, int len) {
  // Make sure to generate a wide variety of characters so we
  // test the boundary conditions for short-key optimizations.
  static const char kTestChars[] = {
    '\0', '\1', 'a', 'b', 'c', 'd', 'e', '\xfd', '\xfe', '\xff'
  };
  std::string result;
  for (int i = 0; i < len; i++) {
    result += kTestChars[rnd->Uniform(sizeof(kTestChars))];
  }
  return result;
}


Slice CompressibleString(Random* rnd, double compressed_fraction,
                                int len, std::string* dst) {
  int raw = static_cast<int>(len * compressed_fraction);
  if (raw < 1) raw = 1;
  std::string raw_data;
  RandomString(rnd, raw, &raw_data);

  // Duplicate the random data until we have filled "len" bytes
  dst->clear();
  while (dst->size() < (size_t)len) {
    dst->append(raw_data);
  }
  dst->resize(len);
  return Slice(*dst);
}
