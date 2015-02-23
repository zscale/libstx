// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-http/HttpLocalFile.h>
#include <xzero-http/HttpLocalFileRepository.h>
#include <xzero-base/sysconfig.h>

#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

namespace xzero {

HttpLocalFile::HttpLocalFile(HttpLocalFileRepository& repo,
                             const std::string& path,
                             const std::string& mimetype)
    : HttpFile(path, mimetype),
      repo_(repo),
      stat_(),
      etag_() {
  update();
}

HttpLocalFile::~HttpLocalFile() {
}

size_t HttpLocalFile::size() const XZERO_NOEXCEPT {
  return stat_.st_size;
}

time_t HttpLocalFile::mtime() const XZERO_NOEXCEPT {
  return stat_.st_mtime;
}

size_t HttpLocalFile::inode() const XZERO_NOEXCEPT {
  return stat_.st_ino;
}

bool HttpLocalFile::isRegular() const XZERO_NOEXCEPT {
  return S_ISREG(stat_.st_mode);
}

const std::string& HttpLocalFile::etag() const {
  // compute ETag response header value on-demand
  if (etag_.empty()) {
    size_t count = 0;
    Buffer buf(256);
    buf.push_back('"');

    if (repo_.etagConsiderMTime_) {
      if (count++) buf.push_back('-');
      buf.push_back(mtime());
    }

    if (repo_.etagConsiderSize_) {
      if (count++) buf.push_back('-');
      buf.push_back(size());
    }

    if (repo_.etagConsiderINode_) {
      if (count++) buf.push_back('-');
      buf.push_back(inode());
    }

    buf.push_back('"');

    etag_ = buf.str();
  }

  return etag_;
}

void HttpLocalFile::update() {
#if 0
  int rv = fstat(fd_, &stat_);
#else
  int rv = stat(path().c_str(), &stat_);
#endif

  if (rv < 0)
    setErrorCode(errno);
  else
    setErrorCode(0);
}

int HttpLocalFile::tryCreateChannel() {
  int flags = O_RDONLY | O_NONBLOCK;

#if 0  // defined(O_NOATIME)
  flags |= O_NOATIME;
#endif

#if defined(O_LARGEFILE)
  flags |= O_LARGEFILE;
#endif

#if defined(O_CLOEXEC)
  flags |= O_CLOEXEC;
#endif

  return ::open(path().c_str(), flags);
}

} // namespace xzero
