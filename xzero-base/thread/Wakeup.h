// This file is part of the "libxzero" project
//   (c) 2009-2015 Christian Parpart <https://github.com/christianparpart>
//   (c) 2014-2015 Paul Asmuth <https://github.com/paulasmuth>
//
// libxzero is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License v3.0. You should have received a
// copy of the GNU General Public License along with this program. If not, see
// <http://www.gnu.org/licenses/>.

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
