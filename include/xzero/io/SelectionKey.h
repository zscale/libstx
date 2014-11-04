// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero/Api.h>
#include <xzero/io/Selectable.h>
#include <memory>

namespace xzero {

class Selector;

/**
 * Abstract Selector handle API.
 *
 * You are to create an instance of an implementation of SelectionKey
 * in order to get notified about I/O events on a system handle,
 * such as InetEndPoint.
 */
class SelectionKey {
 public:
  explicit SelectionKey(Selector* selector) : active_(0), selector_(selector) {}
  virtual ~SelectionKey() {}

  /**
   * Retrieves the current interest.
   */
  virtual int interest() const noexcept = 0;

  /**
   * changes interest.
   */
  virtual void change(int ops) = 0;

  /** Tests whether the associated Selectable is currently ready for reading. */
  bool isReadable() const noexcept { return active_ & Selectable::READ; }

  /** Tests whether the associated Selectable is currently ready for writing. */
  bool isWriteable() const noexcept { return active_ & Selectable::WRITE; }

  /** Retrieves the Selector that created this interest key. */
  Selector* selector() const noexcept { return selector_; }

 protected:
  /**
   * Updates the interest status relative to the current notification.
   *
   * @param activity a bitset of @c Selectable::READ and @c Selectable::WRITE.
   */
  void setActivity(unsigned activity) {
    active_ = activity;
  }

 private:
  unsigned active_;
  Selector* selector_;
};

} // namespace xzero
