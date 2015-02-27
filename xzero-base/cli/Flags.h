// This file is part of the "x0" project
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <xzero-base/Api.h>
#include <xzero-base/cli/FlagType.h>
#include <functional>
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>

namespace xzero {

class IPAddress;

typedef std::pair<FlagType, std::string> FlagValue;

class XZERO_API Flags {
 public:
  Flags();

  std::string getString(const std::string& flag) const;
  long int getNumber(const std::string& flag) const;
  float getFloat(const std::string& flag) const;
  bool getBool(const std::string& flag) const;

  bool isSet(const std::string& flag) const;

  const std::vector<std::string>& getRawArgs() const;

 private:
  std::unordered_map<std::string, FlagValue> set_;
  std::vector<std::string> raw_;
};

}  // namespace xzero
