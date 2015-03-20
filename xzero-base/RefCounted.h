// This file is part of the "libxzero" project
//   (c) 2009-2015 Christian Parpart <https://github.com/christianparpart>
//   (c) 2014-2015 Paul Asmuth <https://github.com/paulasmuth>
//
// libxzero is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License v3.0. You should have received a
// copy of the GNU General Public License along with this program. If not, see
// <http://www.gnu.org/licenses/>.

#pragma once

#include <xzero-base/Api.h>
#include <atomic>

namespace xzero {

class XZERO_API RefCounted {
 public:
  RefCounted();
  virtual ~RefCounted();

  void ref();
  bool unref();

  unsigned refCount() const;

 private:
  std::atomic<unsigned> refCount_;
};

// {{{ RefCounted impl
inline RefCounted::RefCounted()
    : refCount_(0) {
}

inline RefCounted::~RefCounted() {
}

inline unsigned RefCounted::refCount() const {
  return refCount_.load();
}

inline void RefCounted::ref() {
  refCount_++;
}

inline bool RefCounted::unref() {
  if (std::atomic_fetch_sub_explicit(&refCount_, 1u,
                                     std::memory_order_release) == 1) {
    std::atomic_thread_fence(std::memory_order_acquire);
    delete this;
    return true;
  }

  return false;
}
// }}}

} // namespace xzero
