#pragma once

#include <xzero/Api.h>
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
  ThreadedExecutor();
  ~ThreadedExecutor();

  void execute(Task&& task) override;
  void execute(const std::string& name, Task&& task);
  size_t maxConcurrency() const noexcept override;
  std::string toString() const override;
  void joinAll();

 private:
  static void* launchme(void* ptr);

  std::deque<pthread_t> threads_;
  std::mutex mutex_;
};

} // namespace xzero

