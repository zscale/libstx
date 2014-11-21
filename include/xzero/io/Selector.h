// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero/Api.h>
#include <memory>

namespace xzero {

class Selectable;
class SelectionKey;

/**
 * I/O Multiplexer Service API.
 *
 * To perform non-blocking I/O operations generically you want to use an
 * implementation of this interface.
 */
class XZERO_API Selector {
 public:
  explicit Selector(std::function<void(const std::exception&)>&& errorLogger);

  virtual ~Selector() {}

  /**
   * Registers a Selectable @p handle to initially watch on @p ops.
   *
   * @param handle  a Selectable object you want to watch on.
   * @param ops     a bitset of @p READ or @p WRITE readiness-events you want
   *                to get notified for.
   *
   * @return an owned SelectionKey that represents your interest.
   */
  virtual std::unique_ptr<SelectionKey> createSelectable(Selectable* handle,
                                                         unsigned ops) = 0;

  /**
   * Blocks for I/O readines events and notifies their Selectable's.
   */
  virtual void select() = 0;

  /**
   * Breaks the event-loop inside @c select(), if currently running.
   */
  virtual void wakeup() = 0;

 protected:
  void logError(const std::exception& e) const;

 private:
  std::function<void(const std::exception&)> errorLogger_;
};

} // namespace xzero
