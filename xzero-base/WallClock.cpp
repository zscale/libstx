// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/WallClock.h>
#include <xzero-base/DateTime.h>
#include <xzero-base/sysconfig.h>
#include <xzero-base/RuntimeError.h>
#include <sys/time.h>
#include <ctime>

namespace xzero {

class SimpleClock : public WallClock {
 public:
  DateTime get() const override;
};

DateTime SimpleClock::get() const {
  struct timeval tv;
  struct timezone tz;

  int rc = gettimeofday(&tv, &tz);
  if (rc < 0)
    RAISE_ERRNO(errno);

  return DateTime(
      static_cast<double>(tv.tv_sec) +
      TimeSpan::fromMicroseconds(tv.tv_usec).value());
}

#ifdef HAVE_CLOCK_GETTIME
class HighPrecisionClock : public WallClock {
 public:
  explicit HighPrecisionClock(int clkid);
  DateTime get() const override;

 private:
  int clkid_;
};

HighPrecisionClock::HighPrecisionClock(int clkid)
    : clkid_(clkid) {
}

DateTime HighPrecisionClock::get() const {
  timespec ts;
  memset(&ts, 0, sizeof(ts));
  if (clock_gettime(clkid_, &ts) < 0)
    return DateTime(std::time(nullptr));

  return DateTime(
      static_cast<double>(ts.tv_sec) +
      TimeSpan::fromNanoseconds(ts.tv_nsec).value());
}
#endif

WallClock* WallClock::system() {
#ifdef HAVE_CLOCK_GETTIME
  static HighPrecisionClock clock(CLOCK_REALTIME);
  return &clock;
#else
  static SimpleClock bc;
  return &bc;
#endif
}

WallClock* WallClock::monotonic() {
#ifdef HAVE_CLOCK_GETTIME
  static HighPrecisionClock clock(CLOCK_MONOTONIC);
  return &clock;
#else
  static SimpleClock bc;
  return &bc;
#endif
}

} // namespace xzero
