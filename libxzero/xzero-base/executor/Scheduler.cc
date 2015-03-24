// This file is part of the "libxzero" project
//   (c) 2009-2015 Christian Parpart <https://github.com/christianparpart>
//   (c) 2014-2015 Paul Asmuth <https://github.com/paulasmuth>
//
// libxzero is free software: you can redistribute it and/or modify it under
// the terms of the GNU Affero General Public License v3.0.
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <xzero-base/executor/Scheduler.h>

namespace xzero {

Scheduler::Handle::Handle(Task onFire, std::function<void(Handle*)> onCancel)
    : mutex_(),
      onFire_(onFire),
      onCancel_(onCancel),
      isCancelled_(false) {
}

void Scheduler::Handle::cancel() {
  std::lock_guard<std::mutex> lk(mutex_);

  isCancelled_.store(true);

  if (onCancel_) {
    auto cancelThat = std::move(onCancel_);
    cancelThat(this);
  }
}

void Scheduler::Handle::fire() {
  std::lock_guard<std::mutex> lk(mutex_);

  if (!isCancelled_.load()) {
    onFire_();
  }
}

}  // namespace xzero
