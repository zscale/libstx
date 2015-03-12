// This file is part of the "x0" project
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <xzero-base/Api.h>
#include <functional>
#include <string>
#include <memory>

namespace xzero {

class File;

class XZERO_API FileRepository {
 public:
  virtual ~FileRepository();

  virtual std::shared_ptr<File> getFile(
      const std::string& requestPath,
      const std::string& docroot) = 0;

  virtual void listFiles(std::function<bool(const std::string&)> callback) = 0;
  virtual void deleteAllFiles() = 0;
};

}  // namespace xzero
