// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/io/LocalFile.h>
#include <xzero-base/io/LocalFileRepository.h>
#include <xzero-base/io/MemoryMap.h>
#include <xzero-base/io/FileDescriptor.h>
#include <xzero-base/RuntimeError.h>
#include <xzero-base/sysconfig.h>

#include <fstream>
#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

namespace xzero {

LocalFile::LocalFile(LocalFileRepository& repo,
                     const std::string& path,
                     const std::string& mimetype)
    : File(path, mimetype),
      repo_(repo),
      stat_(),
      etag_() {
  update();
}

LocalFile::~LocalFile() {
}

size_t LocalFile::size() const XZERO_NOEXCEPT {
  return stat_.st_size;
}

time_t LocalFile::mtime() const XZERO_NOEXCEPT {
  return stat_.st_mtime;
}

size_t LocalFile::inode() const XZERO_NOEXCEPT {
  return stat_.st_ino;
}

bool LocalFile::isRegular() const XZERO_NOEXCEPT {
  return S_ISREG(stat_.st_mode);
}

const std::string& LocalFile::etag() const {
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

void LocalFile::update() {
  int rv = stat(path().c_str(), &stat_);

  if (rv < 0)
    setErrorCode(errno);
  else
    setErrorCode(0);
}

int LocalFile::tryCreateChannel() {
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

std::unique_ptr<std::istream> LocalFile::createInputChannel() {
  return std::unique_ptr<std::istream>(
      new std::ifstream(path(), std::ios::binary));
}

std::unique_ptr<std::ostream> LocalFile::createOutputChannel() {
  return std::unique_ptr<std::ostream>(
      new std::ofstream(path(), std::ios::binary));
}

std::unique_ptr<MemoryMap> LocalFile::createMemoryMap(bool rw) {
  // use FileDescriptor for auto-closing here, in case of exceptions
  FileDescriptor fd = ::open(path().c_str(), rw ? O_RDWR : O_RDONLY);
  if (fd < 0)
    RAISE_ERRNO(errno);

  return std::unique_ptr<MemoryMap>(new MemoryMap(fd, 0, size(), rw));
}

} // namespace xzero
