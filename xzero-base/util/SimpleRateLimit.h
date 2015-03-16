// This file is part of the "libxzero" project, http://xzero.io/
//   (c) 2009-2015 Christian Parpart <trapni@gmail.com>
//   (c) 2014-2015 Paul Asmuth
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <xzero-base/Api.h>
#include <xzero-base/WallClock.h>
#include <xzero-base/TimeSpan.h>
#include <functional>
#include <string>
#include <stdlib.h>
#include <stdint.h>

namespace xzero {
namespace util {

class XZERO_API SimpleRateLimit {
 public:
  SimpleRateLimit(const TimeSpan& period);

  bool check();

 protected:
  uint64_t period_micros_;
  uint64_t last_micros_;
};

class XZERO_API SimpleRateLimitedFn {
 public:
  SimpleRateLimitedFn(const TimeSpan& period, std::function<void ()> fn);

  void runMaybe();
  void runForce();

 protected:
  SimpleRateLimit limit_;
  std::function<void ()> fn_;
};



} // namespace util
} // namespace xzero
