/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <cstdarg>
#include <fcntl.h>
#include <memory>
#include <stdarg.h>
#include <string>
#include <unistd.h>
#include <stx/buffer.h>
#include <stx/exception.h>
#include <stx/io/FileOutputStream.h>

namespace stx {

std::unique_ptr<FileOutputStream> FileOutputStream::openFile(
    const std::string& file_path,
    int flags /* = O_CREAT | O_TRUNC */,
    int permissions /* = 0666 */) {
  int fd = -1;
  auto fp = file_path.c_str();

  flags |= O_WRONLY;

  if ((flags & O_CREAT) == O_CREAT) {
    fd = open(fp, flags, permissions);
  } else {
    fd = open(fp, flags);
  }

  if (fd < 1) {
    RAISE_ERRNO(kIOError, "error opening file '%s'", fp);
  }

  return std::unique_ptr<FileOutputStream>(new FileOutputStream(fd, true));
}

std::unique_ptr<FileOutputStream> FileOutputStream::fromFileDescriptor(
    int fd,
    bool close_on_destroy /* = false */) {
  std::unique_ptr<FileOutputStream> stream(
      new FileOutputStream(fd, close_on_destroy));

  return stream;
}

std::unique_ptr<FileOutputStream> FileOutputStream::fromFile(File&& file) {
  std::unique_ptr<FileOutputStream> stream(new FileOutputStream(std::move(file)));
  return stream;
}

FileOutputStream::FileOutputStream(
    int fd,
    bool close_on_destroy /* = false */) :
    fd_(fd),
    close_on_destroy_(close_on_destroy) {}

FileOutputStream::FileOutputStream(
    File&& file) :
    FileOutputStream(file.releaseFD(), true) {}

FileOutputStream::~FileOutputStream() {
  if (fd_ >= 0 && close_on_destroy_) {
    close(fd_);
  }
}

size_t FileOutputStream::write(const char* data, size_t size) {
  int bytes_written = -1;

  bytes_written = ::write(fd_, data, size);

  if (bytes_written < 0) {
    RAISE_ERRNO(kIOError, "write() failed");
  }

  return bytes_written;
}

size_t FileOutputStream::printf(const char* format, ...) {
  va_list args;
  va_start(args, format);
  int pos = vdprintf(fd_, format, args);
  va_end(args);

  if (pos < 0) {
    RAISE_ERRNO(kIOError, "vdprintf() failed");
  }

  return pos;
}

} // namespace stx
