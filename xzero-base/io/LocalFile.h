// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero-base/Api.h>
#include <xzero-base/io/File.h>
#include <string>
#include <functional>
#include <unordered_map>
#include <sys/stat.h>

namespace xzero {

class LocalFileRepository;

class XZERO_API LocalFile : public File {
 public:
  LocalFile(LocalFileRepository& repo,
            const std::string& path,
            const std::string& mimetype);
  ~LocalFile();

  const std::string& etag() const override;
  size_t size() const XZERO_NOEXCEPT override;
  time_t mtime() const XZERO_NOEXCEPT override;
  size_t inode() const XZERO_NOEXCEPT override;
  bool isRegular() const XZERO_NOEXCEPT override;
  int tryCreateChannel() override;
  std::unique_ptr<std::istream> createInputChannel() override;
  std::unique_ptr<std::ostream> createOutputChannel() override;
  std::unique_ptr<MemoryMap> createMemoryMap(bool rw = true) override;

  void update();

 private:
  LocalFileRepository& repo_;
  struct stat stat_;
  mutable std::string etag_;
};

} // namespace xzero

