#pragma once

#include <xzero/Api.h>
#include <xzero/TimeSpan.h>
#include <xzero/DateTime.h>
#include <functional>

namespace xzero {

class WallClock;
class Scheduler;

/**
 * Manages a single idle timeout.
 */
class XZERO_API IdleTimeout {
 public:
  IdleTimeout(WallClock* clock, Scheduler* scheduler);
  ~IdleTimeout();

  void setTimeout(TimeSpan value);
  TimeSpan timeout() const;

  void setCallback(std::function<void()>&& cb);
  void clearCallback();

  void activate();
  void deactivate();
  bool isActive() const;

  /**
   * Resets idle timer.
   *
   * Touches the idle-timeout object, effectively resetting the timer back to 0.
   *
   * If this object is not activated nothing will happen.
   */
  void touch();

  /**
   * Retrieves the timespan elapsed since idle timer started or 0 if inactive.
   */
  TimeSpan elapsed() const;

 private:
  void schedule();
  void reschedule();
  void onFired();

 private:
  WallClock* clock_;
  Scheduler* scheduler_;
  TimeSpan timeout_;
  DateTime fired_;
  bool active_;
  std::function<void()> onTimeout_;
};

} // namespace xzero
