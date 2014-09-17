#include <xzero/Api.h>
#include <xzero/DateTime.h>

namespace xzero {

/**
 * Abstract API for retrieving the current system time.
 */
class WallClock {
 public:
  virtual ~WallClock() {}

  virtual DateTime get() const = 0;
};

} // namespace xzero
