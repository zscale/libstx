// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero-base/Api.h>
#include <string>

namespace xzero {

enum class LogLevel {
  None = 0,
  Error = 1,
  Warning = 2,
  Info = 3,
  Debug = 4,
  Trace = 5,

  none = None,
  error = Error,
  warn = Warning,
  info = Info,
  debug = Debug,
  trace = Trace,
};

XZERO_API std::string to_string(LogLevel value);
XZERO_API LogLevel to_loglevel(const std::string& value);

}  // namespace xzero
