// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero/Api.h>
#include <xzero/sysconfig.h>
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

  ev::loop_ref loop() const XZERO_NOEXCEPT { return loop_; }

 private:
  void onWakeup(ev::async&, int);

 private:
  ev::loop_ref loop_;
  ev::async evWakeup_;
};

} // namespace support
} // namespace xzero
