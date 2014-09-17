#pragma once

#include <xzero/Api.h>
#include <xzero/io/Selector.h>
#include <memory>

namespace xzero {

/**
 * Represents a multiplexable selectable resource, such as a file descriptor.
 *
 * Your high-level resource must inherit from this interface in order
 * to be multiplexed by a Selector service.
 */
class XZERO_API Selectable {
 public:
  static const int READ = 1;
  static const int WRITE = 2;

  /**
   * Retrieves the underlying system handle that is used for the actual
   * multiplexing.
   */
  virtual int handle() const noexcept = 0;

  /**
   * Retrieves the Selector this selectable was registered for.
   */
  virtual Selector* selector() const noexcept = 0;

  /**
   * Callback invoked when any of the interested I/O occurred.
   */
  virtual void onSelectable() noexcept = 0;

  /**
   * Convenience helper to register this Selectable on the associated selector.
   *
   * @param ops bitset of events (@c READ and @c WRITE) that you are
   *            interested in.
   * @return    a handle that represents your interest.
   */
  virtual std::unique_ptr<SelectionKey> registerSelectable(unsigned ops);
};

} // namespace xzero

