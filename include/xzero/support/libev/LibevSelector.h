#pragma once

#include <xzero/Api.h>
#include <xzero/io/Selector.h>
#include <ev++.h>

namespace xzero {
namespace support {

/**
 * libev implementation for the Selector API.
 *
 */
class XZERO_API LibevSelector : public Selector {
 public:
  LibevSelector(ev::loop_ref loop);
  ~LibevSelector();

  std::unique_ptr<SelectionKey> createSelectable(Selectable* handle,
                                                 unsigned ops) override;

  void select() override;
  void wakeup() override;

  ev::loop_ref loop() const noexcept { return loop_; }

 private:
  void onWakeup(ev::async&, int);

 private:
  ev::loop_ref loop_;
  ev::async evWakeup_;
};

} // namespace support
} // namespace xzero
