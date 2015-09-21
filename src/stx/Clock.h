#pragma once

#include <stx/UnixTime.h>

namespace stx {

class Clock {
public:
  Clock();
  virtual ~Clock() {}

  virtual void update() = 0;

  UnixTime get() const {
    return theTime_;
  }

  void set(UnixTime thatTime) {
    theTime_ = thatTime;
  }

  UnixTime now() {
    update();
    return get();
  }

  /**
   * System-wide clock that represents the real (wall-clock) time.
   */
  static Clock* realtime();

  /**
   * System-wide clock that represents the real (wall-clock) time, less
   * accourate but faster.
   */
  static Clock* realtimeCoarse();

  /**
   * Retrieves a clock that is not affected by any discontinuous time jumps.
   */
  static Clock* monotonic();

  /**
   * Retrieves a monotonic time that is very fast but less accurate.
   */
  static Clock* monotonicCoarse();

  /**
   * Retrieves a monotonic clock that includes OS-suspend time.
   */
  static Clock* monotonicBootTime();

  /**
   * High-resolution per-process timer from the CPU.
   */
  static Clock* processCpuTime();

  /**
   * High-resolution per-thread timer from the CPU.
   */
  static Clock* threadCpuTime();

private:
  UnixTime theTime_;
};

} // namespace stx
