// This file is part of the "x0" project
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero/Api.h>
#include <xzero/executor/Scheduler.h>
#include <sys/select.h>
#include <list>
#include <deque>
#include <mutex>

namespace xzero {

class WallClock;

class XZERO_API NativeScheduler : public Scheduler {
 public:
  NativeScheduler(
      std::function<void(const std::exception&)> errorLogger,
      WallClock* clock,
      std::function<void()> preInvoke,
      std::function<void()> postInvoke);

  explicit NativeScheduler(
      std::function<void(const std::exception&)> errorLogger,
      WallClock* clock);

  NativeScheduler();

  ~NativeScheduler();

  void execute(Task&& task) override;
  std::string toString() const override;
  HandleRef executeAfter(TimeSpan delay, Task task) override;
  HandleRef executeAt(DateTime dt, Task task) override;
  HandleRef executeOnReadable(int fd, Task task) override;
  HandleRef executeOnWritable(int fd, Task task) override;
  size_t timerCount() override;
  size_t readerCount() override;
  size_t writerCount() override;
  size_t taskCount() override;
  void runLoop() override;
  void runLoopOnce() override;
  void breakLoop() override;

 protected:
  void removeFromTimersList(Handle* handle);
  HandleRef insertIntoTimersList(DateTime dt, HandleRef handle);
  void collectTimeouts(std::vector<HandleRef>* result);

 private:
  WallClock* clock_;
  std::mutex lock_;
  int wakeupPipe_[2];

  Task onPreInvokePending_;
  Task onPostInvokePending_;

  std::deque<Task> tasks_;
  std::list<std::pair<int, HandleRef>> readers_;
  std::list<std::pair<int, HandleRef>> writers_;

  struct Timer {
    DateTime when;
    HandleRef handle;
  };
  std::list<Timer> timers_;
};

} // namespace xzero
