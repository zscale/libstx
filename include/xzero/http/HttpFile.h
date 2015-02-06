// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero/Api.h>
#include <string>
#include <functional>
#include <unordered_map>
#include <sys/stat.h>

namespace xzero {

/**
 * HTTP servable file.
 *
 * @see HttpFileHandler
 */
class XZERO_API HttpFile {
 public:
  HttpFile(const HttpFile&) = delete;
  HttpFile& operator=(const HttpFile&) = delete;

  HttpFile(const std::string& path, const std::string& mimetype, bool mtime,
           bool size, bool inode);

  ~HttpFile();

  bool isError() const XZERO_NOEXCEPT { return errno_ != 0; }
  int errorCode() const XZERO_NOEXCEPT { return errno_; }

  const std::string& path() const { return path_; }
  std::string filename() const;
  const std::string& mimetype() const { return mimetype_; }
  const std::string& etag() const;
  const std::string& lastModified() const;

  size_t size() const XZERO_NOEXCEPT { return stat_.st_size; }
  time_t mtime() const XZERO_NOEXCEPT { return stat_.st_mtime; }
  operator const struct stat*() const { return &stat_; }
  const struct stat* operator->() const { return &stat_; }

  bool isRegular() const XZERO_NOEXCEPT { return S_ISREG(stat_.st_mode); }

  void update();

  /** Creates a file descriptor for this file. */
  int tryCreateChannel();

 private:
  std::string path_;
  int errno_;
  std::string mimetype_;
  bool etagConsiderMTime_;
  bool etagConsiderSize_;
  bool etagConsiderINode_;
  mutable std::string etag_;
  mutable std::string lastModified_;
  struct stat stat_;
};

} // namespace xzero
