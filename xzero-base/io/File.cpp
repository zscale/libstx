// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/io/File.h>
#include <xzero-base/Buffer.h>
#include <xzero-base/logging.h>
#include <xzero-base/sysconfig.h>

#include <ctime>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#if 1
#define TRACE(level, msg...) logTrace("File", msg)
#else
#define TRACE(level, msg...) do {} while (0)
#endif

namespace xzero {

File::File(const std::string& path, const std::string& mimetype)
    : path_(path),
      mimetype_(mimetype),
      errno_(0),
      lastModified_() {
  TRACE(2, "(%s).ctor", path_.c_str());
}

File::~File() {
  TRACE(2, "(%s).dtor", path_.c_str());
}

std::string File::filename() const {
  size_t n = path_.rfind('/');
  return n != std::string::npos ? path_.substr(n + 1) : path_;
}

const std::string& File::lastModified() const {
  // build Last-Modified response header value on-demand
  if (lastModified_.empty()) {
    time_t modificationTime = mtime();
    struct tm tm;
    if (gmtime_r(&modificationTime, &tm)) {
      char buf[256];

      if (std::strftime(buf, sizeof(buf), "%a, %d %b %Y %T GMT", &tm) != 0) {
        lastModified_ = buf;
      }
    }
  }

  return lastModified_;
}

} // namespace xzero
