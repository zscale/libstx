// This file is part of the "libxzero" project, http://xzero.io/
//   (c) 2009-2015 Christian Parpart <trapni@gmail.com>
//   (c) 2014-2015 Paul Asmuth
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/Application.h>
#include <xzero-base/thread/SignalHandler.h>
#include <xzero-base/logging/LogAggregator.h>
#include <xzero-base/logging/LogTarget.h>
#include <xzero-base/RuntimeError.h>
#include <stdlib.h>

namespace xzero {

void Application::init() {
  Application::installGlobalExceptionHandler();

  thread::SignalHandler::ignore(SIGPIPE);

  // well, when you detach from the terminal, you're garanteed to not get one.
  // unless someone sends it explicitely (so why ignoring then)?
  thread::SignalHandler::ignore(SIGHUP);
}

void Application::logToStderr(LogLevel loglevel) {
  LogAggregator::get().setLogTarget(LogTarget::console());
  LogAggregator::get().setLogLevel(loglevel);
}

static void globalEH() {
  try {
    throw;
  } catch (const std::exception& e) {
    logAndAbort(e);
  } catch (...) {
    // d'oh
    fprintf(stderr, "Unhandled foreign exception caught.\n");
    abort();
  }
}

void Application::installGlobalExceptionHandler() {
  std::set_terminate(&globalEH);
  std::set_unexpected(&globalEH);
}

} // namespace xzero
