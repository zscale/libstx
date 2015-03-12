// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero-base/Api.h>
#include <xzero-base/Buffer.h>
#include <functional>
#include <string>

namespace xzero {

/**
 * HTTP servable file.
 *
 * @see FileHandler
 * @see LocalFile, MemoryFile
 * @see FileRepository
 */
class XZERO_API File {
 public:
  File(const File&) = delete;
  File& operator=(const File&) = delete;

  File(const std::string& path, const std::string& mimetype);

  virtual ~File();

  std::string filename() const;
  const std::string& path() const { return path_; }
  const std::string& mimetype() const { return mimetype_; }
  const std::string& lastModified() const;

  virtual const std::string& etag() const = 0;

  virtual size_t size() const XZERO_NOEXCEPT = 0;
  virtual time_t mtime() const XZERO_NOEXCEPT = 0;
  virtual size_t inode() const XZERO_NOEXCEPT = 0;
  virtual bool isRegular() const XZERO_NOEXCEPT = 0;

  /** Creates a file descriptor for this file. */
  virtual int tryCreateChannel() = 0;

  void setErrorCode(int ec) { errno_ = ec; }
  int errorCode() const XZERO_NOEXCEPT { return errno_; }

 private:
  std::string path_;
  std::string mimetype_;

  int errno_;

 protected:
  mutable std::string lastModified_;
};

} // namespace xzero
