// This file is part of the "libxzero" project
//   (c) 2009-2015 Christian Parpart <https://github.com/christianparpart>
//   (c) 2014-2015 Paul Asmuth <https://github.com/paulasmuth>
//
// libxzero is free software: you can redistribute it and/or modify it under
// the terms of the GNU Affero General Public License v3.0.
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <xzero-base/Api.h>
#include <xzero-base/TimeSpan.h>
#include <xzero-base/DateTime.h>
#include <xzero-base/executor/Scheduler.h>
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
  Scheduler::HandleRef handle_;
};

} // namespace xzero
