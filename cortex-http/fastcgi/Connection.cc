// This file is part of the "libcortex" project
//   (c) 2009-2015 Christian Parpart <https://github.com/christianparpart>
//   (c) 2014-2015 Paul Asmuth <https://github.com/paulasmuth>
//
// libcortex is free software: you can redistribute it and/or modify it under
// the terms of the GNU Affero General Public License v3.0.
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <cortex-http/fastcgi/Connection.h>
#include <cortex-http/HttpChannel.h>
#include <cortex-http/HttpBufferedInput.h>
#include <cortex-http/HttpDateGenerator.h>
#include <cortex-http/HttpResponseInfo.h>
#include <cortex-http/HttpResponse.h>
#include <cortex-http/HttpRequest.h>
#include <cortex-http/BadMessage.h>
#include <cortex-base/net/Connection.h>
#include <cortex-base/net/EndPoint.h>
#include <cortex-base/net/EndPointWriter.h>
#include <cortex-base/executor/Executor.h>
#include <cortex-base/logging.h>
#include <cortex-base/RuntimeError.h>
#include <cortex-base/WallClock.h>
#include <cortex-base/sysconfig.h>
#include <cassert>
#include <cstdlib>

namespace cortex {
namespace http {
namespace fastcgi {

/* TODO
 *
 * - how to handle BadMessage exceptions
 * - test this class for multiplexed requests
 * - test this class for multiplexed responses
 * - test this class for early client aborts
 * - test this class for early server aborts
 */

#ifndef NDEBUG
#define TRACE(msg...) logTrace("fastcgi", msg)
#else
#define TRACE(msg...) do {} while (0)
#endif

class HttpFastCgiTransport : public HttpTransport { // {{{
 public:
  HttpFastCgiTransport(int id, EndPointWriter* writer);
  virtual ~HttpFastCgiTransport();

  void abort() override;
  void completed() override;
  void send(HttpResponseInfo&& responseInfo, const BufferRef& chunk, CompletionHandler onComplete) override;
  void send(HttpResponseInfo&& responseInfo, Buffer&& chunk, CompletionHandler onComplete) override;
  void send(HttpResponseInfo&& responseInfo, FileRef&& chunk, CompletionHandler onComplete) override;
  void send(Buffer&& chunk, CompletionHandler onComplete) override;
  void send(FileRef&& chunk, CompletionHandler onComplete) override;
  void send(const BufferRef& chunk, CompletionHandler onComplete) override;

 private:
  Generator generator_;
};

HttpFastCgiTransport::HttpFastCgiTransport(int id, EndPointWriter* writer)
    : generator_(id, nullptr, writer) {
}

HttpFastCgiTransport::~HttpFastCgiTransport() {
}

void HttpFastCgiTransport::abort() { // TODO
  // channel_->response()->setBytesTransmitted(generator_.bytesTransmitted());
  // channel_->responseEnd();
  // endpoint()->close();
}

void HttpFastCgiTransport::completed() {
  // TODO: generator_.generateTrailer(channel_->response()->trailers());
}

void HttpFastCgiTransport::send(HttpResponseInfo&& responseInfo, const BufferRef& chunk, CompletionHandler onComplete) {
  generator_.generateResponse(responseInfo, chunk);
}

void HttpFastCgiTransport::send(HttpResponseInfo&& responseInfo, Buffer&& chunk, CompletionHandler onComplete) {
  generator_.generateResponse(responseInfo, std::move(chunk));
}

void HttpFastCgiTransport::send(HttpResponseInfo&& responseInfo, FileRef&& chunk, CompletionHandler onComplete) {
  generator_.generateResponse(responseInfo, std::move(chunk));
}

void HttpFastCgiTransport::send(Buffer&& chunk, CompletionHandler onComplete) {
  generator_.generateBody(std::move(chunk));
}

void HttpFastCgiTransport::send(FileRef&& chunk, CompletionHandler onComplete) {
  generator_.generateBody(std::move(chunk));
}

void HttpFastCgiTransport::send(const BufferRef& chunk, CompletionHandler onComplete) {
  generator_.generateBody(chunk);
}
// }}}
class HttpFastCgiChannel : public HttpChannel { // {{{
 public:
  HttpFastCgiChannel(HttpTransport* transport,
                     const HttpHandler& handler,
                     std::unique_ptr<HttpInput>&& input,
                     size_t maxRequestUriLength,
                     size_t maxRequestBodyLength,
                     HttpDateGenerator* dateGenerator,
                     HttpOutputCompressor* outputCompressor);
  ~HttpFastCgiChannel();
};

HttpFastCgiChannel::~HttpFastCgiChannel() {
  // we own the transport
  delete transport_;
}
// }}}

Connection::Connection(EndPoint* endpoint,
                       Executor* executor,
                       const HttpHandler& handler,
                       HttpDateGenerator* dateGenerator,
                       HttpOutputCompressor* outputCompressor,
                       size_t maxRequestUriLength,
                       size_t maxRequestBodyLength)
    : cortex::Connection(endpoint, executor),
      handler_(handler),
      maxRequestUriLength_(maxRequestUriLength),
      maxRequestBodyLength_(maxRequestBodyLength),
      dateGenerator_(dateGenerator),
      outputCompressor_(outputCompressor),
      inputBuffer_(),
      inputOffset_(0),
      parser_(
          std::bind(&Connection::onCreateChannel, this, std::placeholders::_1),
          std::bind(&Connection::onUnknownPacket, this, std::placeholders::_1, std::placeholders::_2),
          std::bind(&Connection::onAbortRequest, this, std::placeholders::_1)),
      channels_(),
      writer_(),
      onComplete_() {

  TRACE("%p ctor", this);
}

Connection::~Connection() {
  TRACE("%p dtor", this);
}

void Connection::onOpen() {
  TRACE("%p onOpen", this);
  cortex::Connection::onOpen();

  // TODO support TCP_DEFER_ACCEPT here
#if 0
  if (connector()->deferAccept())
    onFillable();
  else
    wantFill();
#else
  wantFill();
#endif
}

void Connection::onClose() {
  TRACE("%p onClose", this);
  cortex::Connection::onClose();
}

void Connection::setInputBufferSize(size_t size) {
  TRACE("%p setInputBufferSize(%zu)", this, size);
  inputBuffer_.reserve(size);
}

void Connection::onFillable() {
  TRACE("%p onFillable", this);

  TRACE("%p onFillable: calling fill()", this);
  if (endpoint()->fill(&inputBuffer_) == 0) {
    TRACE("%p onFillable: fill() returned 0", this);
    // RAISE("client EOF");
    endpoint()->close();
    return;
  }

  parseFragment();
}

void Connection::parseFragment() {
  TRACE("parseFragment: calling parseFragment (%zu into %zu)",
        inputOffset_, inputBuffer_.size());
  size_t n = parser_.parseFragment(inputBuffer_.ref(inputOffset_));
  TRACE("parseFragment: called (%zu into %zu) => %zu",
        inputOffset_, inputBuffer_.size(), n);
  inputOffset_ += n;
}

void Connection::onFlushable() {
  TRACE("%p onFlushable", this);

  const bool complete = writer_.flush(endpoint());

  if (complete) {
    TRACE("%p onFlushable: completed. (%s)", this, (onComplete_ ? "onComplete cb set" : "onComplete cb not set"));

    if (onComplete_) {
      TRACE("%p onFlushable: invoking completion callback", this);
      auto callback = std::move(onComplete_);
      onComplete_ = nullptr;
      callback(true);
    }
  } else {
    // continue flushing as we still have data pending
    wantFlush();
  }
}

void Connection::onInterestFailure(const std::exception& error) {
  TRACE("%p onInterestFailure(%s): %s", this, typeid(error).name(), error.what());

  // TODO: improve logging here, as this eats our exception here.
  // e.g. via (factory or connector)->error(error);
  logError("fastcgi", error);

  auto callback = std::move(onComplete_);
  onComplete_ = nullptr;

  // notify the callback that we failed doing something wrt. I/O.
  if (callback) {
    TRACE("%p onInterestFailure: invoking onComplete(false)", this);
    callback(false);
  }

  endpoint()->close();
}

HttpListener* Connection::onCreateChannel(int request) {
  return createChannel(request);
}

void Connection::onUnknownPacket(int request, int record) {
}

void Connection::onAbortRequest(int request) {
  removeChannel(request);
}

HttpChannel* Connection::createChannel(int request) {
  if (channels_.find(request) != channels_.end()) {
    RAISE(IllegalStateError,
          "FastCGI channel with ID %i already present.",
          request);
  }

  try {
    std::unique_ptr<HttpFastCgiChannel> channel(new HttpFastCgiChannel(
       new HttpFastCgiTransport(request, &writer_),
       handler_,
       std::unique_ptr<HttpInput>(new HttpBufferedInput()),
       maxRequestUriLength_,
       maxRequestBodyLength_,
       dateGenerator_,
       outputCompressor_));

    channel->request()->setRemoteIP(endpoint()->remoteIP());

    return (channels_[request] = std::move(channel)).get();
  } catch (...) {
    removeChannel(request);
    throw;
  }
}

void Connection::removeChannel(int request) {
  auto i = channels_.find(request);
  if (i != channels_.end()) {
    channels_.erase(i);
  }
}

} // namespace fastcgi
} // namespace http
} // namespace cortex
