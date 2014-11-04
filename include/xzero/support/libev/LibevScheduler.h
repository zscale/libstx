// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero/Api.h>
#include <xzero/executor/Scheduler.h>
#include <list>
#include <ev++.h>

namespace xzero {
namespace support {

class XZERO_API LibevScheduler : public Scheduler {
 public:
  explicit LibevScheduler(ev::loop_ref loop);
  ~LibevScheduler();

  void execute(Task&& task) override;
  void schedule(TimeSpan delay, Task&& task) override;
  size_t maxConcurrency() const noexcept override;
  std::string toString() const override;

 private:
  struct TaskInfo;

  ev::loop_ref loop_;
  std::list<TaskInfo*> pending_;
};

} // namespace support
} // namespace xzero
