// This file is part of the "x0" project
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/cli/Flags.h>
#include <xzero-base/net/IPAddress.h>
#include <xzero-base/RuntimeError.h>

namespace xzero {

Flags::Flags() {
}

std::string Flags::getString(const std::string& flag) const {
  auto i = set_.find(flag);
  if (i == set_.end())
    return "";

  return i->second.second;
}

long int Flags::getNumber(const std::string& flag) const {
  auto i = set_.find(flag);
  if (i == set_.end())
    return 0;

  return std::stoi(i->second.second);
}

float Flags::getFloat(const std::string& flag) const {
  auto i = set_.find(flag);
  if (i == set_.end())
    return 0.0f;

  return std::stof(i->second.second);
}

bool Flags::getBool(const std::string& flag) const {
  auto i = set_.find(flag);
  if (i == set_.end())
    return false;

  return !i->second.second.empty();
}

const std::vector<std::string>& Flags::getRawArgs() const {
  return raw_;
}

}  // namespace xzero
