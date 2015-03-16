// This file is part of the "libxzero" project, http://xzero.io/
//   (c) 2009-2015 Christian Parpart <trapni@gmail.com>
//   (c) 2014-2015 Paul Asmuth
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/util/SimpleRateLimit.h>

namespace xzero {
namespace util {

SimpleRateLimit::SimpleRateLimit(
    const TimeSpan& period) :
    period_micros_(period.microseconds()),
    last_micros_(0) {}

bool SimpleRateLimit::check() {
  auto now = WallClock::monotonic()->get().unixMicroseconds();

  if (now - last_micros_ >= period_micros_) {
    last_micros_ = now;
    return true;
  } else {
    return false;
  }
}

SimpleRateLimitedFn::SimpleRateLimitedFn(
    const TimeSpan& period,
    std::function<void ()> fn) :
    limit_(period),
    fn_(fn) {}

void SimpleRateLimitedFn::runMaybe() {
  if (limit_.check()) {
    fn_();
  }
}

void SimpleRateLimitedFn::runForce() {
  fn_();
}

}
}

