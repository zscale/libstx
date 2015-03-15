// This file is part of the "libxzero" project, http://xzero.io/
//   (c) 2009-2015 Christian Parpart <trapni@gmail.com>
//   (c) 2014-2015 Paul Asmuth
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/Api.h>
#include <xzero-base/logging/LogLevel.h>

namespace xzero {

class XZERO_API Application {
 public:
  static void init();

  static void logToStderr(LogLevel loglevel = LogLevel::Info);

  static void installGlobalExceptionHandler();
};

} // namespace xzero
