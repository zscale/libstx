#pragma once

#include <xzero/Api.h>
#include <list>
#include <memory>

namespace xzero {

class Buffer;
class BufferRef;
class FileRef;

/**
 * Adds an interface to HTTP response output filters.
 *
 * @see HttpOutput
 * @see HttpResponse
 */
class XZERO_API HttpOutputFilter {
 public:
  virtual void filter(const BufferRef& input, Buffer* output, bool last) = 0;

  static void applyFilters(
    const std::list<std::shared_ptr<HttpOutputFilter>>& filters,
    const BufferRef& input, Buffer* output, bool last);

  static void applyFilters(
    const std::list<std::shared_ptr<HttpOutputFilter>>& filters,
    const FileRef& input, Buffer* output, bool last);
};

} // namespace xzero
