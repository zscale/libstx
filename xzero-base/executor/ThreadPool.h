// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero-base/Api.h>
#include <xzero-base/sysconfig.h>
#include <xzero-base/executor/Executor.h>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <atomic>
#include <deque>

namespace xzero {

/**
 * Standard thread-safe thread pool.
 */
class XZERO_API ThreadPool : public Executor {
 public:
  /**
   * Initializes this thread pool as many threads as CPU cores are available.
   */
  ThreadPool() : ThreadPool(nullptr) {}

  /**
   * Initializes this thread pool.
   * @param num_threads number of threads to allocate.
   */
  explicit ThreadPool(size_t num_threads) : ThreadPool(num_threads, nullptr) {}

  /**
   * Initializes this thread pool as many threads as CPU cores are available.
   */
  explicit ThreadPool(std::function<void(const std::exception&)> eh);

  /**
   * Initializes this thread pool.
   *
   * @param num_threads number of threads to allocate.
   */
  ThreadPool(size_t num_threads, std::function<void(const std::exception&)> eh);

  ~ThreadPool();

  static size_t processorCount();

  /**
   * Retrieves the number of pending tasks.
   */
  size_t pendingCount() const;

  /**
   * Retrieves the number of threads currently actively running a task.
   */
  size_t activeCount() const;

  /**
   * Notifies all worker threads to stop after their current job (if any).
   */
  void stop();

  /**
   * Waits until all jobs are processed.
   */
  void wait();

  // overrides
  void execute(Task task) override;
  std::string toString() const override;

 private:
  void work(int workerId);

 private:
  bool active_;
  std::deque<std::thread> threads_;
  mutable std::mutex mutex_;
  std::condition_variable condition_;
  std::deque<Task> pendingTasks_;
  std::atomic<size_t> activeTasks_;
};

} // namespace xzero
