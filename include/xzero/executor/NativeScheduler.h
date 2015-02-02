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

class NativeKey;

class XZERO_API NativeScheduler : public Scheduler {
 public:
  NativeScheduler(
      std::function<void(const std::exception&)> errorLogger,
      std::function<void()> preInvoke,
      std::function<void()> postInvoke);

  explicit NativeScheduler(
      std::function<void(const std::exception&)> errorLogger);

  NativeScheduler();

  ~NativeScheduler();

  void execute(Task&& task) override;
  size_t maxConcurrency() const XZERO_NOEXCEPT override;
  std::string toString() const override;
  HandleRef executeAfter(TimeSpan delay, Task task) override;
  HandleRef executeAt(DateTime dt, Task task) override;
  HandleRef executeOnReadable(int fd, Task task) override;
  HandleRef executeOnWritable(int fd, Task task) override;

  void runLoopOnce() override;
  void breakLoop() override;

 protected:
  void runTaskQueue();

 private:
  std::list<NativeKey*> keys_;
  std::mutex lock_;
  int wakeupPipe_[2];

  std::function<void()> onPreInvokePending_;
  std::function<void()> onPostInvokePending_;

  std::deque<Task> tasks_;
  std::list<std::pair<int, HandleRef>> readers_;
  std::list<std::pair<int, HandleRef>> writers_;
};

} // namespace xzero
