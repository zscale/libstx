#pragma once

#include <xzero/Api.h>
#include <xzero/executor/Executor.h>
#include <deque>
#include <mutex>
#include <thread>
#include <condition_variable>

namespace xzero {

/**
 * Standard thread-safe thread pool.
 */
class XZERO_API ThreadPool : public Executor {
 public:
  /**
   * Initializes this thread pool.
   *
   * @param num_threads number of threads to allocate, -1 for number of
   *                    available processors.
   */
  explicit ThreadPool(size_t num_threads = -1);
  ~ThreadPool();

  void execute(Task&& task) override;
  size_t maxConcurrency() const noexcept override;

 private:
  std::deque<std::thread> threads_;
  std::mutex mutex_;
  std::condition_variable condition_;
};

} // namespace xzero
