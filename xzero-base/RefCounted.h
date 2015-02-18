// This file is part of the "x0" project
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
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
