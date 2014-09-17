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
};

} // namespace xzero
