// This file is part of the "libxzero" project, http://xzero.io/
//   (c) 2009-2015 Christian Parpart <trapni@gmail.com>
//   (c) 2014-2015 Paul Asmuth
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/Api.h>
#include <condition_variable>
#include <atomic>
#include <mutex>
#include <list>

namespace xzero {
namespace thread {

class XZERO_API Wakeup {
 public:
  Wakeup();

  /**
   * Block the current thread and wait for the next wakeup event
   */
  void waitForNextWakeup();
  void waitForFirstWakeup();
  void waitForWakeup(long generation);

  void wakeup();
  void onWakeup(long generation, std::function<void()> callback);

  long generation() const;

 protected:
  std::mutex mutex_;
  std::condition_variable condvar_;
  std::atomic<long> gen_;
  std::list<std::function<void()>> callbacks_;
};

} // namespace thread
} // namespace xzero
