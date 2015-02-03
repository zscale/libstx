// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero/Api.h>
#include <xzero/sysconfig.h>
#include <xzero/executor/Scheduler.h>
#include <list>
#include <ev++.h>

namespace xzero {

namespace support {

class XZERO_API LibevScheduler : public Scheduler {
 public:
  LibevScheduler(ev::loop_ref loop,
                 std::function<void(const std::exception&)>&& eh = nullptr);
  ~LibevScheduler();

  void execute(Task&& task) override;
  void schedule(TimeSpan delay, Task&& task) override;
  size_t maxConcurrency() const XZERO_NOEXCEPT override;
  std::string toString() const override;

  void wakeup();

 private:
  void onWakeup(ev::async&, int);

  struct TaskInfo;

  ev::loop_ref loop_;
  ev::async evWakeup_;
  std::list<TaskInfo*> pending_;
};

} // namespace support
} // namespace xzero
