#pragma once

#include <xzero/Api.h>
#include <xzero/Buffer.h>
#include <xzero/CompletionHandler.h>
#include <functional>
#include <deque>

namespace xzero {

class FileRef;
class HttpChannel;

/**
 * Represents the HTTP response body producer API.
 */
class XZERO_API HttpOutput {
 public:
  explicit HttpOutput(HttpChannel* channel);
  virtual ~HttpOutput();

  virtual void recycle();

  /**
   * Writes given C-string @p cstr to the client.
   *
   * @param cstr the null-terminated data chunk to write to the client.
   * @param completed Callback to invoke after completion.
   */
  virtual void write(const char* cstr, CompletionHandler&& completed = nullptr);

  /**
   * Writes given buffer.
   *
   * @param data the data chunk to write to the client.
   * @param completed Callback to invoke after completion.
   *
   * The buffer ref must stay valid until fully written to the client.
   * The callee will not create any copy of it.
   */
  virtual void write(const BufferRef& data, CompletionHandler&& completed = nullptr);

  /**
   * Writes given buffer.
   *
   * @param data the data chunk to write to the client.
   * @param completed Callback to invoke after completion.
   */
  virtual void write(Buffer&& data, CompletionHandler&& completed = nullptr);

  /**
   * Writes the data received from the given file descriptor @p file.
   *
   * @param file file ref handle
   * @param completed Callback to invoke after completion.
   */
  virtual void write(FileRef&& file, CompletionHandler&& completed = nullptr);

  /**
   * Invoked by HttpResponse::completed().
   */
  virtual void completed();

 private:
  HttpChannel* channel_;
};

}  // namespace xzero
