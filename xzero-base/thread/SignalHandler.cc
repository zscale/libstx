// This file is part of the "libxzero" project, http://xzero.io/
//   (c) 2009-2015 Christian Parpart <trapni@gmail.com>
//   (c) 2014-2015 Paul Asmuth
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/thread/SignalHandler.h>
#include <signal.h>

namespace xzero {
namespace thread {

void SignalHandler::ignore(int signum) {
  ::signal(signum, SIG_IGN);
}

void SignalHandler::ignoreSIGHUP() {
  ignore(SIGHUP);
}

void SignalHandler::ignoreSIGPIPE() {
  ignore(SIGPIPE);
}

}  // namespace thread
}  // namespace xzero
