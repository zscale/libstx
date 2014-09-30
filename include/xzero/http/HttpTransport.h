#pragma once

#include <xzero/http/HeaderFieldList.h>
#include <xzero/http/HttpStatus.h>
#include <xzero/net/Connection.h>
#include <xzero/CompletionHandler.h>
#include <xzero/Buffer.h>
#include <memory>

namespace xzero {

class FileRef;
class HttpResponseInfo;

/**
 * HTTP transport layer API.
 *
 * Implemements the wire transport protocol for HTTP messages without
 * any semantics.
 *
 * For HTTP/1 for example it is <b>RFC 7230</b>.
 */
class XZERO_API HttpTransport : public Connection {
 public:
  explicit HttpTransport(std::shared_ptr<EndPoint> endpoint);

  /**
   * Cancels communication completely.
   */
  virtual void abort() = 0;

  /**
   * Invoked when the currently generated response has been fully transmitted.
   */
  virtual void completed() = 0;

  /**
   * Initiates sending a response to the client.
   *
   * @param responseInfo HTTP response meta informations.
   * @param chunk response body data chunk.
   * @param onComplete callback invoked when sending chunk is succeed/failed.
   *
   * @note You must ensure the data chunk is available until sending completed!
   */
  virtual void send(HttpResponseInfo&& responseInfo, const BufferRef& chunk,
                    CompletionHandler&& onComplete) = 0;

  /**
   * Initiates sending a response to the client.
   *
   * @param responseInfo HTTP response meta informations.
   * @param chunk response body data chunk.
   * @param onComplete callback invoked when sending chunk is succeed/failed.
   */
  virtual void send(HttpResponseInfo&& responseInfo, Buffer&& chunk,
                    CompletionHandler&& onComplete) = 0;

  /**
   * Transfers this data chunk to the output stream.
   *
   * @param chunk response body chunk
   * @param onComplete callback invoked when sending chunk is succeed/failed.
   */
  virtual void send(Buffer&& chunk, CompletionHandler&& onComplete) = 0;

  /**
   * Transfers this file data chunk to the output stream.
   *
   * @param chunk response body chunk represented as a file.
   * @param onComplete callback invoked when sending chunk is succeed/failed.
   */
  virtual void send(FileRef&& chunk, CompletionHandler&& onComplete) = 0;

  /**
   * Transfers this data chunk to the output stream.
   *
   * @param chunk response body chunk
   * @param onComplete callback invoked when sending chunk is succeed/failed.
   */
  virtual void send(const BufferRef& chunk, CompletionHandler&& onComplete) = 0;
};

inline HttpTransport::HttpTransport(std::shared_ptr<EndPoint> endpoint)
    : Connection(endpoint) {
}

}  // namespace xzero2
