/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_UTIL_LOGLEVEL_H
#define _libstx_UTIL_LOGLEVEL_H
#include "stx/UnixTime.h"
#include "stx/stdtypes.h"

namespace stx {

enum class LogLevel {
  kEmergency = 9000,
  kAlert = 8000,
  kCritical = 7000,
  kError = 6000,
  kWarning = 5000,
  kNotice = 4000,
  kInfo = 3000,
  kDebug = 2000,
  kTrace = 1000
};

}
#endif
