#include <cortex-http/fastcgi/Parser.h>
#include <cortex-http/HttpListener.h>
#include <cortex-base/RuntimeError.h>
#include <cortex-base/StringUtil.h>
#include <cortex-base/logging.h>

namespace cortex {
namespace http {
namespace fastcgi {

#define TRACE(msg...) do { \
  logTrace("fastcgi", "Parser: " msg); \
} while (0)

// {{{ Parser::StreamState impl
Parser::StreamState::StreamState()
    : listener(nullptr),
      expectedContentLength(0),
      actualContentLength(0),
      totalBytesReceived(0),
      paramsFullyReceived(false),
      params(),
      body() {
}

Parser::StreamState::~StreamState() {
}

void Parser::StreamState::reset() {
  listener = nullptr;
  expectedContentLength = 0;
  actualContentLength = 0;
  totalBytesReceived = 0;
  paramsFullyReceived = false;
  params.clear();
  body.clear();
}
// }}}

Parser::StreamState& Parser::registerStreamState(int requestId) {
  if (streams_.find(requestId)  != streams_.end())
    RAISE(RuntimeError,
          "FastCGI stream with requestID %d already available.",
          requestId);

  return streams_[requestId];
}

void Parser::removeStreamState(int requestId) {
  auto i = streams_.find(requestId);
  if (i != streams_.end()) {
    streams_.erase(i);
  }
}

void Parser::StreamState::onParam(
    const char *name, size_t nameLen,
    const char *value, size_t valueLen) {
  params.emplace_back(std::string(name, nameLen), std::string(value, valueLen));
}

Parser::Parser(
    std::function<HttpListener*(int requestId)> onCreateChannel,
    std::function<void(int requestId, int recordId)> onUnknownPacket,
    std::function<void(int requestId)> onAbortRequest)
    : onCreateChannel_(onCreateChannel),
      onUnknownPacket_(onUnknownPacket),
      onAbortRequest_(onAbortRequest) {
}

size_t Parser::parseFragment(const Record& record) {
  return parseFragment(BufferRef((const char*) &record, record.size()));
}

size_t Parser::parseFragment(const BufferRef& chunk) {
  size_t readOffset = 0;

  // process each fully received packet ...
  while (readOffset + sizeof(fastcgi::Record) <= chunk.size()) {
    const fastcgi::Record* record =
        reinterpret_cast<const fastcgi::Record*>(chunk.data() + readOffset);

    if (chunk.size() - readOffset < record->size()) {
      break; // not enough bytes to process (next) full packet
    }

    readOffset += record->size();

    process(record);
  }
  return readOffset;
}

void Parser::process(const fastcgi::Record* record) {
  switch (record->type()) {
    case fastcgi::Type::BeginRequest:
      return beginRequest(static_cast<const fastcgi::BeginRequestRecord*>(record));
    case fastcgi::Type::AbortRequest:
      return abortRequest(static_cast<const fastcgi::AbortRequestRecord*>(record));
    case fastcgi::Type::Params:
      return streamParams(record);
    case fastcgi::Type::StdIn:
      return streamStdIn(record);
    case fastcgi::Type::StdOut:
      return streamStdOut(record);
    case fastcgi::Type::StdErr:
      return streamStdErr(record);
    case fastcgi::Type::Data:
      return streamData(record);
    case fastcgi::Type::GetValues:
      //return getValues(...);
    default:
      ;//write<fastcgi::UnknownTypeRecord>(record->type(), record->requestId());
  }
}

void Parser::beginRequest(const fastcgi::BeginRequestRecord* record) {
  StreamState& stream = streams_[record->requestId()];
  stream.totalBytesReceived += record->size();
  stream.listener = onCreateChannel_(record->requestId());
}

void Parser::streamParams(const fastcgi::Record* record) {
  StreamState& stream = streams_[record->requestId()];
  stream.totalBytesReceived += record->size();

  if (record->contentLength()) {
    stream.processParams(record->content(), record->contentLength());
  } else {
    stream.paramsFullyReceived = true;
    if (stream.actualContentLength <= stream.expectedContentLength) {
      // an empty record marks the end of the param-stream.
      // so we should hand over the request unless a request body is expected.
      handleRequest(stream);
    } else {
      //TRACE("streamParams: ignoring (%lu, %lu)",
      //      r->requestContentLength_, r->requestContent_.size());
    }
  }
}

void Parser::streamStdIn(const fastcgi::Record* record) {
  StreamState& stream = streams_[record->requestId()];
  stream.totalBytesReceived += record->size();

  BufferRef content(record->content(), record->contentLength());

  if (stream.listener->onMessageContent(content) == false)
    return;

  if (stream.paramsFullyReceived &&
      stream.actualContentLength >= stream.expectedContentLength &&
      record->contentLength() == 0) {
    stream.listener->onMessageEnd();
  }
}

void Parser::streamStdOut(const fastcgi::Record* record) {
  StreamState& stream = streams_[record->requestId()];
  stream.totalBytesReceived += record->size();
}

void Parser::streamStdErr(const fastcgi::Record* record) {
  StreamState& stream = streams_[record->requestId()];
  stream.totalBytesReceived += record->size();
}

void Parser::streamData(const fastcgi::Record* record) {
  StreamState& stream = streams_[record->requestId()];
  stream.totalBytesReceived += record->size();
}

void Parser::abortRequest(const fastcgi::AbortRequestRecord* record) {
  StreamState& stream = streams_[record->requestId()];
  stream.totalBytesReceived += record->size();

  onAbortRequest_(record->requestId());

  removeStreamState(record->requestId());
}

void Parser::handleRequest(StreamState& stream) {
  BufferRef method;
  BufferRef entity;
  HttpVersion version = HttpVersion::VERSION_1_0;

  if (!stream.listener->onMessageBegin(method, entity, version))
    return;

  for (const auto& param: stream.params) {
    if (!stream.listener->onMessageHeader(
        BufferRef(param.first),
        BufferRef(param.second))) {
      return;
    }
  }

  if (!stream.listener->onMessageHeaderEnd()) {
    return;
  }

  if (!stream.body.empty()) {
    stream.listener->onMessageContent(stream.body);
  }
}

} // namespace fastcgi
} // namespace http
} // namespace cortex
