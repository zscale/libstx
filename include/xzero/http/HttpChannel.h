#pragma once

#include <xzero/Api.h>
#include <xzero/CompletionHandler.h>
#include <xzero/http/HttpListener.h>
#include <xzero/http/HttpHandler.h>

namespace xzero {

class FileRef;
class HttpTransport;
class HttpRequest;
class HttpResponse;
class HttpInput;
class HttpOutput;

enum class HttpChannelState {
  IDLE,     //!< not doing a shit, bro.
  READING,  //!< reading request info
  READY,    //!< ready for handling the request
  HANDLING, //!< currently handling the request
  DONE,     //!< handling request done
};

/**
 * Semantic HTTP message exchange layer.
 *
 * An HttpChannel implements the semantic layer ontop of the transport layer.
 *
 * @see HttpTransport
 */
class XZERO_API HttpChannel : public HttpListener {
 public:
  HttpChannel(HttpTransport* transport, const HttpHandler& handler,
              std::unique_ptr<HttpInput>&& input);
  ~HttpChannel();

  /**
   * Resets the channel state.
   *
   * After invokation this channel is as it would have been just instanciated.
   */
  virtual void reset();

  /**
   * Sends a response body chunk @p data.
   *
   * @param data body chunk
   * @param onComplete callback invoked when sending chunk is succeed/failed.
   *
   * The response will auto-commit the response status line and
   * response headers if not done yet.
   */
  void send(const BufferRef& data, CompletionHandler&& onComplete);

  /**
   * Sends a response body chunk as defined by @p file.
   *
   * @param file the system file handle containing the data to be sent to the
   *             client.
   * @param onComplete callback invoked when sending chunk is succeed/failed.
   *
   * The response will auto-commit the response status line and
   * response headers if not done yet.
   */
  void send(FileRef&& file, CompletionHandler&& onComplete);

  /**
   * Sends an 100-continue intermediate response message.
   */
  void send100Continue();

  /**
   * Retrieves the request object for the current request.
   */
  HttpRequest* request() const { return request_.get(); }

  /**
   * Retrieves the response object for the current response.
   */
  HttpResponse* response() const { return response_.get(); }

  /**
   * Invoked by HttpResponse::completed() in to inform this channel about its
   * completion.
   */
  void completed();

  // HttpListener overrides
  bool onMessageBegin(const BufferRef& method, const BufferRef& entity,
                      int versionMajor, int versionMinor) override;
  bool onMessageHeader(const BufferRef& name, const BufferRef& value) override;
  bool onMessageHeaderEnd() override;
  bool onMessageContent(const BufferRef& chunk) override;
  bool onMessageEnd() override;
  void onProtocolError(HttpStatus code, const std::string& message) override;

 protected:
  virtual std::unique_ptr<HttpOutput> createOutput();
  void handleRequest();

 protected:
  HttpChannelState state_;
  HttpTransport* transport_;
  std::unique_ptr<HttpRequest> request_;
  std::unique_ptr<HttpResponse> response_;
  HttpHandler handler_;
};

}  // namespace xzero
