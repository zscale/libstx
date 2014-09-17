#pragma once

#include <xzero/Api.h>
#include <xzero/Buffer.h>
#include <xzero/DateTime.h>

namespace xzero {

class WallClock;

/**
 * API to generate an HTTP conform Date response header field value.
 */
class XZERO_API HttpDateGenerator {
 public:
  explicit HttpDateGenerator(WallClock* clock);

  WallClock* clock() const { return clock_; }
  void setClock(WallClock* clock) { clock_ = clock; }

  void update();
  void fill(Buffer* target);

 private:
  WallClock* clock_;
  DateTime current_;
  Buffer buffer_;
};

} // namespace xzero
