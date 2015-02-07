// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero/Api.h>
#include <xzero/executor/SafeCall.h>
#include <xzero/sysconfig.h>

#include <exception>
#include <deque>
#include <functional>
#include <string>

namespace xzero {

/**
 * Closure Execution Service API.
 *
 * Defines an interface for executing tasks. The implementer
 * can distinguish in different execution models, such as threading,
 * sequential or inline execution.
 *
 * @see DirectExecutor
 * @see ThreadPool
 */
class XZERO_API Executor : protected SafeCall {
 public:
  explicit Executor(std::function<void(const std::exception&)>&& eh);
  virtual ~Executor();

  typedef std::function<void()> Task;

  using SafeCall::setExceptionHandler;

  /**
   * Executes given task.
   */
  virtual void execute(Task&& task) = 0;

  /**
   * Retrieves a human readable name of this executor (for introspection only).
   */
  virtual std::string toString() const = 0;
};

} // namespace xzero
