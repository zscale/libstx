// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero-base/Api.h>
#include <xzero-base/DateTime.h>

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

  /**
   * Retrieves a process-global monotonic clock that never jumps back in time.
   */
  static WallClock* monotonic();
};

} // namespace xzero
