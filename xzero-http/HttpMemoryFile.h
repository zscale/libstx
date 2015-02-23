// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero-http/Api.h>
#include <xzero-http/HttpFile.h>
#include <xzero-base/Buffer.h>
#include <string>

namespace xzero {

class HttpMemoryFile : public HttpFile {
 public:
  /** Initializes a "not found" file. */
  HttpMemoryFile();

  /** Initializes a memory backed file. */
  HttpMemoryFile(const std::string& path, const char* data, size_t length,
                 const std::string& mimetype);
  ~HttpMemoryFile();

  const std::string& etag() const override;
  size_t size() const XZERO_NOEXCEPT override;
  time_t mtime() const XZERO_NOEXCEPT override;
  size_t inode() const XZERO_NOEXCEPT override;
  bool isRegular() const XZERO_NOEXCEPT override;
  int tryCreateChannel() override;

 private:
  Buffer data_;
  time_t mtime_;
  std::string etag_;
  std::string shm_path_;
  int memfd_;
};

} // namespace xzero

