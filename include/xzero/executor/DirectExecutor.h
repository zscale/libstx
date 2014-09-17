#pragma once

#include <xzero/Api.h>
#include <xzero/executor/Executor.h>
#include <deque>

namespace xzero {

/**
 * Executor to directly invoke the tasks being passed by the caller.
 *
 * @note Not thread-safe.
 */
class XZERO_API DirectExecutor : public Executor {
 public:
  explicit DirectExecutor(bool recursive = false);

  void execute(Task&& task) override;
  std::string toString() const override;
  size_t maxConcurrency() const noexcept override;

  /** Tests whether this executor is currently running some task. */
  bool isRunning() const { return running_ > 0; }

  /** Tests whether this executor allows recursive execution of tasks. */
  bool isRecursive() const { return recursive_; }

  /** Sets wether recursive execution is allowed or flattened. */
  void setRecursive(bool value) { recursive_ = value; }

  /** Retrieves number of deferred tasks. */
  size_t backlog() const { return deferred_.size(); }

 private:
  bool recursive_;
  int running_;
  std::deque<Task> deferred_;
};

} // namespace xzero
