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
