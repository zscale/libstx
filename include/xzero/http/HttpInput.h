#pragma once

#include <xzero/Api.h>

namespace xzero {

class Buffer;
class BufferRef;
class HttpInputListener;

/**
 * Abstract HTTP message body consumer API.
 */
class XZERO_API HttpInput {
 public:
  HttpInput();
  virtual ~HttpInput();

  /**
   * Reads some data into the end of @p result.
   */
  virtual int read(Buffer* result) = 0;

  /**
   * Reads a single line and stores it in @p result without trailing linefeed.
   */
  virtual size_t readLine(Buffer* result) = 0;

  /**
   * Registers a callback interface to get notified when input data is
   * available.
   *
   * @param listener the callback interface to invoke when input data is
   *                 available.
   */
  void setListener(HttpInputListener* listener);

  /**
   * Retrieves the HttpInputListener that is being associated with this input.
   */
  HttpInputListener* listener() const noexcept { return listener_; }

  /**
   * Internally invoked to pass some input chunk to this layer.
   *
   * @param chunk the raw request body data chunk.
   *
   * @internal
   */
  virtual void onContent(const BufferRef& chunk) = 0;

 private:
  HttpInputListener* listener_;
};

}  // namespace xzero
