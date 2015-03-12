// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/io/MemoryFile.h>
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

// TODO: use O_TMPFILE if available, otherwise shm_open if ok, otherwise mktemp

MemoryFile::MemoryFile()
    : File("", ""),
      mtime_(0),
      etag_(),
      shm_path_() {
}

MemoryFile::MemoryFile(
    const std::string& path,
    const std::string& mimetype,
    const BufferRef& data,
    DateTime mtime)
    : File(path, mimetype),
      mtime_(mtime.unixtime()),
      size_(data.size()),
      etag_(std::to_string(hash::FNV<uint64_t>().hash(data.data(), data.size()))),
      shm_path_(path) {

  StringUtil::replaceAll(&shm_path_, "/", "\%2f"); // TODO: URL-escape instead

  if (shm_path_.size() >= NAME_MAX)
    RAISE(RuntimeError, "MemoryFile's path must not exceed NAME_MAX.");

  FileDescriptor fd = shm_open(shm_path_.c_str(), O_RDWR | O_CREAT, 0600);
  if (fd < 0)
    RAISE_ERRNO(errno);
#if XZERO_OS_DARWIN
  printf("1\n");
  ssize_t n = write(fd, data.data(), data.size());
  if (n < 0)
    RAISE_ERRNO(errno);

  printf("2\n");
  if (lseek(fd, 0, SEEK_SET) < 0)
    RAISE_ERRNO(errno);
  printf("3\n");
#else
  if (ftruncate(fd, size()) < 0)
    RAISE_ERRNO(errno);

  ssize_t n = pwrite(fd, data.data(), data.size(), 0);
  if (n < 0)
    RAISE_ERRNO(errno);
#endif

  if (static_cast<size_t>(n) != data.size())
    RAISE(RuntimeError, "Couldn't write it all.");
}

MemoryFile::~MemoryFile() {
  //shm_unlink(shm_path_.c_str());
}

const std::string& MemoryFile::etag() const {
  return etag_;
}

size_t MemoryFile::size() const XZERO_NOEXCEPT {
  return size_;
}

time_t MemoryFile::mtime() const XZERO_NOEXCEPT {
  return mtime_;
}

size_t MemoryFile::inode() const XZERO_NOEXCEPT {
  return getpid();
}

bool MemoryFile::isRegular() const XZERO_NOEXCEPT {
  return true;
}

int MemoryFile::tryCreateChannel() {
  if (shm_path_.empty()) {
    setErrorCode(ENOENT);
    return -1;
  }

  return shm_open(shm_path_.c_str(), O_RDONLY, 0600);
}

} // namespace xzero
