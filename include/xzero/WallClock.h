#include <xzero/Api.h>
#include <xzero/DateTime.h>

namespace xzero {

/**
 * Abstract API for retrieving the current system time.
 */
class XZERO_API WallClock {
 public:
  virtual ~WallClock() {}

  virtual DateTime get() const = 0;

  /**
   * Retrieves a global unique system clock using the standard C runtime.
   */
  static WallClock* system();
};

} // namespace xzero
