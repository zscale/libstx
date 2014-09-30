#pragma once

#include <xzero/Api.h>
#include <xzero/http/HttpVersion.h>
#include <xzero/http/HttpStatus.h>
#include <xzero/http/HttpOutput.h>
#include <xzero/http/HeaderFieldList.h>
#include <memory>

namespace xzero {

class EndPointWriter;
class HttpDateGenerator;
class HttpInfo;
class HttpRequestInfo;
class HttpResponseInfo;


namespace http1 {

/**
 * Generates HTTP/1 messages.
 *
 * @todo how to support sendfile() alike optimizations
 */
class XZERO_API HttpGenerator {
 public:
  explicit HttpGenerator(HttpDateGenerator* dateGenerator);

  /** resets any runtime state. */
  void recycle();

  /**
   * Generates an HTTP request message.
   *
   * @param info HTTP request message info.
   * @param chunk HTTP message body chunk.
   * @param last Boolean value, indicating that this @p chunk is the last one.
   * @param output Target to write the generated (partial) HTTP message to.
   */
  void generateRequest(const HttpRequestInfo& info, Buffer&& chunk,
                       bool last, EndPointWriter* output);
  void generateRequest(const HttpRequestInfo& info, const BufferRef& chunk,
                       bool last, EndPointWriter* output);

  /**
   * Generates an HTTP response message.
   *
   * @param info HTTP response message info.
   * @param chunk HTTP message body chunk.
   * @param last Boolean value, indicating that this @p chunk is the last one.
   * @param output Target to write the generated (partial) HTTP message to.
   */
  void generateResponse(const HttpResponseInfo& info, Buffer&& chunk,
                        bool last, EndPointWriter* output);
  void generateResponse(const HttpResponseInfo& info, const BufferRef& chunk,
                        bool last, EndPointWriter* output);

  /**
   * Generates an HTTP message body chunk.
   *
   * @param chunk HTTP message body chunk.
   * @param last Boolean value, indicating that this @p chunk is the last one.
   * @param output Target to write the generated (partial) HTTP message to.
   */
  void generateBody(Buffer&& chunk, bool last, EndPointWriter* output);
  void generateBody(const BufferRef& chunk, bool last, EndPointWriter* output);

  /**
   * Generates an HTTP message body chunk.
   *
   * @param chunk HTTP message body chunk, represented as a file.
   * @param last Boolean value, indicating that this @p chunk is the last one.
   * @param output Target to write the generated (partial) HTTP message to.
   */
  void generateBody(FileRef&& chunk, bool last, EndPointWriter* output);

  /**
   * Retrieves the number of bytes pending for the content.
   */
  size_t pendingContentLength() const noexcept { return contentLength_; }

  /**
   * Retrieves boolean indicating whether chunked response is generated.
   */
  bool isChunked() const noexcept { return chunked_; }

 private:
  void generateRequestLine(const HttpRequestInfo& info);
  void generateResponseLine(const HttpResponseInfo& info);
  void generateHeaders(const HttpInfo& info);
  void generateResponseInfo(const HttpResponseInfo& info,
                            EndPointWriter* output);
  void flushBuffer(EndPointWriter* output);

 private:
  HttpDateGenerator* dateGenerator_;
  size_t contentLength_;
  bool chunked_;
  Buffer buffer_;
};

}  // namespace http1
}  // namespace xzero
