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

// Since OS/X doesn't support SHM in a way Linux does, we need to work around it.
#if XZERO_OS_DARWIN
#  define XZERO_MEMORYFILE_USE_TMPFILE
#else
// Use TMPFILE here, too (for now), because create*Channel would fail otherwise.
// So let's create a posix_filebuf
//#  define XZERO_MEMORYFILE_USE_SHM
#  define XZERO_MEMORYFILE_USE_TMPFILE
#endif

MemoryFile::MemoryFile()
    : File("", ""),
      mtime_(0),
      inode_(0),
      etag_(),
      fspath_() {
}

MemoryFile::MemoryFile(
    const std::string& path,
    const std::string& mimetype,
    const BufferRef& data,
    DateTime mtime)
    : File(path, mimetype),
      mtime_(mtime.unixtime()),
      inode_(0),
      size_(data.size()),
      etag_(std::to_string(data.hash())),
      fspath_(),
      fd_(-1) {
#if defined(XZERO_MEMORYFILE_USE_TMPFILE)
  fspath_ = FileUtil::joinPaths(FileUtil::tempDirectory(), "memfile.XXXXXXXX");

  fd_ = mkstemp(const_cast<char*>(fspath_.c_str()));
  if (fd_ < 0)
    RAISE_ERRNO(errno);

  if (ftruncate(fd_, size()) < 0)
    RAISE_ERRNO(errno);

  ssize_t n = pwrite(fd_, data.data(), data.size(), 0);
  if (n < 0)
    RAISE_ERRNO(errno);
#else
  // TODO: URL-escape instead
  fspath_ = path;
  StringUtil::replaceAll(&fspath_, "/", "\%2f");

  if (fspath_.size() >= NAME_MAX)
    RAISE(RuntimeError, "MemoryFile's path must not exceed NAME_MAX.");

  FileDescriptor fd = shm_open(fspath_.c_str(), O_RDWR | O_CREAT, 0600);
  if (fd < 0)
    RAISE_ERRNO(errno);

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
#if defined(XZERO_MEMORYFILE_USE_TMPFILE)
  if (fd_ >= 0) {
    ::close(fd_);
  }
#else
  shm_unlink(fspath_.c_str());
#endif
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
  return inode_;
}

bool MemoryFile::isRegular() const XZERO_NOEXCEPT {
  return true;
}

int MemoryFile::tryCreateChannel() {
#if defined(XZERO_MEMORYFILE_USE_TMPFILE)
  if (fd_ < 0) {
    errno = ENOENT;
    return -1;
  }

  return dup(fd_);
#else
  return shm_open(fspath_.c_str(), O_RDONLY, 0600);
#endif
}

} // namespace xzero
