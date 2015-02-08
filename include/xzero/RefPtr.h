// This file is part of the "x0" project
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <xzero/Api.h>
#include <atomic>

namespace xzero {

template<typename T>
class XZERO_API RefPtr {
 public:
  RefPtr(T* obj);
  RefPtr(RefPtr<T>&& other);
  RefPtr(const RefPtr<T>& other);
  RefPtr<T>& operator=(const RefPtr<T>& other);
  RefPtr(std::nullptr_t);
  ~RefPtr();

  T* get() { return obj_; }
  T* operator->() { return obj_; }
  T& operator*() { return *obj_; }

  T* release();

 private:
  T* obj_;
};

// type alias, to keep API kind of in sync with libfnord
template<typename T>
using AutoPtr = RefPtr<T>;

// {{{ RefPtr impl
template<typename T>
RefPtr<T>::RefPtr(T* obj)
    : obj_(obj) {
  if (obj_) {
    obj_->ref();
  }
}

template<typename T>
RefPtr<T>::RefPtr(const RefPtr<T>& other)
    : obj_(other.obj_) {
  if (obj_) {
    obj_->ref();
  }
}

template<typename T>
RefPtr<T>::RefPtr(RefPtr<T>&& other)
    : obj_(other.obj_) {
  other.obj_ = nullptr;
}

template<typename T>
RefPtr<T>& RefPtr<T>::operator=(const RefPtr<T>& other) {
  if (obj_)
    obj_->unref();

  obj_ = other.obj_;

  if (obj_)
    obj_->ref();

  return *this;
}

template<typename T>
RefPtr<T>::RefPtr(std::nullptr_t)
    : obj_(nullptr) {
}

template<typename T>
RefPtr<T>::~RefPtr() {
  if (obj_) {
    obj_->unref();
  }
}

template<typename T>
T* RefPtr<T>::release() {
  T* t = obj_;
  obj_ = nullptr;
  return t;
}
//}}}

} // namespace xzero
