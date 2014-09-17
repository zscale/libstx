#pragma once

#include <xzero/Api.h>

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
class XZERO_API Executor {
 public:
  virtual ~Executor() {}

  typedef std::function<void()> Task;

  /**
   * Executes given task.
   */
  virtual void execute(Task&& task) = 0;

  /**
   * Retrieves the maximum number of possible concurrently running tasks.
   */
  virtual size_t maxConcurrency() const noexcept = 0;

  /**
   * Retrieves a human readable name of this executor (for introspection only).
   */
  virtual std::string toString() const = 0;
};

} // namespace xzero
