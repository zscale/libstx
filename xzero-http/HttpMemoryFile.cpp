// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-http/HttpMemoryFile.h>
#include <xzero-base/Buffer.h>
#include <xzero-base/hash/FNV.h>
#include <xzero-base/RuntimeError.h>
#include <xzero-base/sysconfig.h>

#include <ctime>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

namespace xzero {

HttpMemoryFile::HttpMemoryFile()
    : HttpFile("", ""),
      data_(),
      mtime_(0),
      etag_(),
      shm_path_(),
      memfd_(-1) {
}

HttpMemoryFile::HttpMemoryFile(
    const std::string& path, const char* data, size_t length,
    const std::string& mimetype)
    : HttpFile(path, mimetype),
      data_(),
      mtime_(time(nullptr)),
      etag_(std::to_string(hash::FNV<uint64_t>().hash(data, length))),
      shm_path_(path),
      memfd_(-1) {

  if (shm_path_.size() >= NAME_MAX)
    RAISE(RuntimeError, "HttpMemoryFile's path must not exceed NAME_MAX.");

  data_.push_back(data, length);
}

HttpMemoryFile::~HttpMemoryFile() {
  if (memfd_ >= 0) {
    shm_unlink(shm_path_.c_str());
    ::close(memfd_);
  }
}

const std::string& HttpMemoryFile::etag() const {
  return etag_;
}

size_t HttpMemoryFile::size() const XZERO_NOEXCEPT {
  return data_.size();
}

time_t HttpMemoryFile::mtime() const XZERO_NOEXCEPT {
  return mtime_;
}

size_t HttpMemoryFile::inode() const XZERO_NOEXCEPT {
  return getpid();
}

bool HttpMemoryFile::isRegular() const XZERO_NOEXCEPT {
  return true;
}

int HttpMemoryFile::tryCreateChannel() {
  if (shm_path_.empty()) {
    setErrorCode(ENOENT);
    return -1;
  }

  if (memfd_ < 0) {
    memfd_ = shm_open(shm_path_.c_str(), O_RDONLY, 0440);
  }

  return memfd_;
}

} // namespace xzero
