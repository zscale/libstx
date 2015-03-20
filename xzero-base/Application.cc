// This file is part of the "libxzero" project
//   (c) 2009-2015 Christian Parpart <https://github.com/christianparpart>
//   (c) 2014-2015 Paul Asmuth <https://github.com/paulasmuth>
//
// libxzero is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License v3.0. You should have received a
// copy of the GNU General Public License along with this program. If not, see
// <http://www.gnu.org/licenses/>.

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
