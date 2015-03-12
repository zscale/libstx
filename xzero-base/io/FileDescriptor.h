// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <xzero-base/Api.h>

namespace xzero {

/**
 * Represents a system file descriptor that gets automatically closed.
 */
class XZERO_API FileDescriptor {
 public:
  FileDescriptor(int fd) : fd_(fd) {}
  FileDescriptor(FileDescriptor&& fd) : fd_(fd.release()) {}
  ~FileDescriptor() { close(); }

  FileDescriptor& operator=(FileDescriptor&& fd);

  FileDescriptor(const FileDescriptor& fd) = delete;
  FileDescriptor& operator=(const FileDescriptor& fd) = delete;

  int get() const noexcept { return fd_; }
  operator int() const noexcept { return fd_; }

  int release();
  void close();

 private:
  int fd_;
};

}  // namespace xzero
