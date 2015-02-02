// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero/Api.h>
#include <xzero/sysconfig.h>
#include <xzero/executor/Executor.h>
#include <deque>
#include <pthread.h>
#include <mutex>

namespace xzero {

/**
 * Executor Service using threads.
 *
 * Every executed task will be getting its own dedicated
 * system thread.
 */
class XZERO_API ThreadedExecutor : public Executor {
 public:
  ThreadedExecutor() : ThreadedExecutor(nullptr) {}
  ThreadedExecutor(std::function<void(const std::exception&)>&& eh);
  ~ThreadedExecutor();

  void execute(const std::string& name, Task&& task);

  void execute(Task&& task) override;
  std::string toString() const override;
  void joinAll();

 private:
  static void* launchme(void* ptr);

  std::deque<pthread_t> threads_;
  std::mutex mutex_;
};

} // namespace xzero

