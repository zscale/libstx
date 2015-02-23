// This file is part of the "x0" project
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <xzero-http/Api.h>
#include <memory>

namespace xzero {

class HttpFile;
typedef std::shared_ptr<HttpFile> HttpFileRef;

class XZERO_HTTP_API HttpFileRepository {
 public:
  virtual ~HttpFileRepository() {}

  virtual HttpFileRef getFile(const std::string& requestPath,
                              const std::string& docroot) = 0;
};

}  // namespace xzero
