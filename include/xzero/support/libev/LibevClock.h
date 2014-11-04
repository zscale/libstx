// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero/Api.h>
#include <xzero/WallClock.h>

namespace xzero {
namespace support {

class XZERO_API LibevClock : public WallClock {
 public:
  explicit LibevClock(ev::loop_ref loop);

  DateTime get() const override;

 private:
  ev::loop_ref loop_;
};

} // namespace support
} // namespace xzero
