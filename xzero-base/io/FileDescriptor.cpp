// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/io/FileDescriptor.h>
#include <unistd.h>

namespace xzero {

FileDescriptor& FileDescriptor::operator=(FileDescriptor&& fd) {
  close();
  fd_ = fd.release();

  return *this;
}

int FileDescriptor::release() {
  int fd = fd_;
  fd_ = -1;
  return fd;
}

void FileDescriptor::close() {
  if (fd_ >= 0) {
    ::close(fd_);
    fd_ = -1;
  }
}

}  // namespace xzero
