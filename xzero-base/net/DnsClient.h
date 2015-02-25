// This file is part of the "x0" project
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero-base/Api.h>
#include <xzero-base/net/IPAddress.h>
#include <unordered_map>
#include <string>
#include <mutex>
#include <vector>

namespace xzero {

/**
 * DNS client API.
 */
class XZERO_API DnsClient {
 public:
  IPAddress resolve(const std::string& name);
  std::vector<IPAddress> resolveAll(const std::string& name);
  void clearCache();

 private:
  std::unordered_map<std::string, std::vector<IPAddress>> cache_;
  std::mutex cacheMutex_;
};

}  // namespace xzero
