// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero-base/Api.h>
#include <string>
#include <unordered_map>

namespace xzero {

/**
 * Log Target Interface.
 *
 * Logging target implementations (such as a console logger or syslog logger)
 * must implement this interface.
 */
class XZERO_API LogTarget {
 public:
  virtual ~LogTarget() {}

  virtual void trace(const std::string& msg) = 0;
  virtual void debug(const std::string& msg) = 0;
  virtual void info(const std::string& msg) = 0;
  virtual void warn(const std::string& msg) = 0;
  virtual void error(const std::string& msg) = 0;

  static LogTarget* console(); // standard console logger
  static LogTarget* syslog();  // standard syslog logger
};

}  // namespace xzero
