// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-http/HttpMemoryFile.h>
#include <xzero-base/Buffer.h>
#include <xzero-base/hash/FNV.h>
#include <xzero-base/io/FileUtil.h>
#include <xzero-base/StringUtil.h>
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
      mtime_(0),
      etag_(),
      shm_path_() {
}

HttpMemoryFile::HttpMemoryFile(
    const std::string& path, const char* data, size_t length,
    const std::string& mimetype)
    : HttpFile(path, mimetype),
      mtime_(time(nullptr)), // TODO: pass time
      size_(length),
      etag_(std::to_string(hash::FNV<uint64_t>().hash(data, length))),
      shm_path_(path) {

  StringUtil::replaceAll(&shm_path_, "/", "\%2f"); // TODO: URL-escape instead

  if (shm_path_.size() >= NAME_MAX)
    RAISE(RuntimeError, "HttpMemoryFile's path must not exceed NAME_MAX.");

  FileDescriptor fd = shm_open(shm_path_.c_str(), O_RDWR | O_CREAT, 0600);
  if (fd < 0)
    RAISE_ERRNO(errno);

  if (ftruncate(fd, size()) < 0)
    RAISE_ERRNO(errno);

  ssize_t n = pwrite(fd, data, length, 0);
  if (n < 0)
    RAISE_ERRNO(errno);

  if (static_cast<size_t>(n) != length)
    RAISE(RuntimeError, "Couldn't write it all.");
}

HttpMemoryFile::~HttpMemoryFile() {
  //shm_unlink(shm_path_.c_str());
}

const std::string& HttpMemoryFile::etag() const {
  return etag_;
}

size_t HttpMemoryFile::size() const XZERO_NOEXCEPT {
  return size_;
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

  return shm_open(shm_path_.c_str(), O_RDONLY, 0600);
}

} // namespace xzero
