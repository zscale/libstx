#pragma once

#include <xzero/Api.h>
#include <unordered_map>
#include <string>

namespace xzero {

class HttpRequest;
class HttpResponse;

/**
 * HTTP response output compression.
 */
class XZERO_API HttpOutputCompressor {
 public:
  HttpOutputCompressor();
  ~HttpOutputCompressor();

  void addMimeType(const std::string& value);
  bool containsMimeType(const std::string& value) const;

  void setMinSize(size_t value);
  size_t minSize() const noexcept { return minSize_; }

  void setMaxSize(size_t value);
  size_t maxSize() const noexcept { return maxSize_; }

  void setCompressionLevel(int value) { level_ = value; }
  int compressionLevel() const noexcept { return level_; }

  /**
   * Injects a preCommit handler to automatically add output compression.
   */
  void inject(HttpRequest* request, HttpResponse* response);

  /**
   * Adds output compression to @p response if @p request set to accept any.
   */
  void postProcess(HttpRequest* request, HttpResponse* response);

 private:
  class ZlibFilter;
  class DeflateFilter;
  class GzipFilter;
  class Bzip2Filter;

  size_t minSize_;
  size_t maxSize_;
  int level_;
  std::unordered_map<std::string, int> contentTypes_;
};

} // namespace xzero
