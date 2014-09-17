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
