#pragma once

#include <xzero/Api.h>
#include <list>
#include <memory>

namespace xzero {

class Buffer;
class BufferRef;
class FileRef;

/**
 * I/O Filter API.
 */
class XZERO_API Filter {
 public:
  /**
   * Applies this filter to the given @p input.
   *
   * @param input the input data this filter to apply to.
   * @param output the output to store the filtered data to.
   * @param last indicator whether or not this is the last data chunk in the
   *             stream.
   */
  virtual void filter(const BufferRef& input, Buffer* output, bool last) = 0;

  static void applyFilters(
    const std::list<std::shared_ptr<Filter>>& filters,
    const BufferRef& input, Buffer* output, bool last);

  static void applyFilters(
    const std::list<std::shared_ptr<Filter>>& filters,
    const FileRef& input, Buffer* output, bool last);
};

} // namespace xzero
