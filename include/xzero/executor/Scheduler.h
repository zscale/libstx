// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <xzero/Api.h>
#include <xzero/TimeSpan.h>
#include <xzero/DateTime.h>
#include <xzero/executor/Executor.h>
#include <vector>
#include <functional>
#include <memory>

namespace xzero {

/**
 * Interface for scheduling tasks.
 */
class XZERO_API Scheduler : public Executor {
 public:
  /**
   * it basically supports 2 actions, fire and termination.
   */
  class Handle {
   public:
    Handle(Task onFire, std::function<void(Handle*)> onCancel)
        : onFire_(onFire),
          onCancel_(onCancel) {
    }

    ~Handle() {
    }

    void cancel() {
      if (onCancel_) {
        auto cancelThat = std::move(onCancel_);
        cancelThat(this);
      }
    }

    void fire() {
      onFire_();
    }

   private:
    std::function<void(Handle*)> onCancel_;
    Task onFire_;
  };

  typedef std::shared_ptr<Handle> HandleRef;

  Scheduler(std::function<void(const std::exception&)>&& eh)
      : Executor(std::move(eh)) {}

  /**
   * Schedules given task to be run after given delay.
   *
   * @param task the actual task to be executed.
   * @param delay the timespan to wait before actually executing the task.
   */
  virtual HandleRef executeAfter(TimeSpan delay, Task task) = 0;

  /**
   * Runs given task at given time.
   */
  virtual HandleRef executeAt(DateTime dt, Task task) = 0;

  /**
   * Runs given task when given selectable is non-blocking readable.
   */
  virtual HandleRef executeOnReadable(int fd, Task task) = 0;

  /**
   * Runs given task when given selectable is non-blocking writable.
   */
  virtual HandleRef executeOnWritable(int fd, Task task) = 0;

  /**
   * Runs the event loop exactly once, possibly blocking until an event is
   * fired..
   */
  virtual void runLoopOnce() = 0;

  /**
   * Breaks loop in case it is blocking while waiting for an event.
   */
  virtual void breakLoop() = 0;

 protected:
  void safeCallEach(std::vector<HandleRef>& handles) {
    for (HandleRef& handle: handles)
      safeCall(std::bind(&Handle::fire, handle));
  }

  void safeCallEach(const std::deque<Task>& tasks) {
    for (Task task: tasks) {
      safeCall(task);
    }
  }

};

}  // namespace xzero
