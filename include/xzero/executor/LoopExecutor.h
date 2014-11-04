// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero/Api.h>
#include <xzero/executor/Executor.h>

namespace xzero {

/**
 * Loop-Executor, that runs all enqueued tasks within the same loop.
 */
class XZERO_API LoopExecutor : public Executor {
 public:
  /**
   * Blocks the current thread until cancel() is invoked and runs all tasks 
   * that get scheduled.
   */
  virtual void run() = 0;

  /**
   * Cancels the currently active run() call and forces it to return to its
   * caller.
   */
  virtual void cancel() = 0;
};

} // namespace xzero

