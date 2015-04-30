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
      headers(),
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
  headers.reset();
  body.clear();
}

void Parser::StreamState::onParam(
    const char *name, size_t nameLen,
    const char *value, size_t valueLen) {
  std::string nam(name, nameLen);
  std::string val(value, valueLen);

  if (StringUtil::beginsWith(nam, "HTTP_")) {
    // strip off "HTTP_" prefix and replace all '_' with '-'
    nam = nam.substr(5);
    StringUtil::replaceAll(&nam, "_", "-");

    headers.push_back(nam, val);
    return;
  }
 
  // non-HTTP-header parameters
  params.emplace_back(nam, val);

  // SERVER_PORT
  // SERVER_ADDR
  // SERVER_PROTOCOL
  // SERVER_SOFTWARE
  // SERVER_NAME (aka. Host header)
  //
  // REMOTE_ADDR
  // REMOTE_PORT
  // REMOTE_IDENT
  //
  // HTTPS
  // CONTENT_TYPE
  // CONTENT_LENGTH
  // DOCUMENT_ROOT
  // SCRIPT_FILENAME
  // AUTH_TYPE
  // QUERY_STRING
  // REQUEST_URI
  // PATH_TRANSLATED
  // PATH_INFO
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

Parser::Parser(
    std::function<HttpListener*(int requestId)> onCreateChannel,
    std::function<void(int requestId, int recordId)> onUnknownPacket,
    std::function<void(int requestId)> onAbortRequest,
    std::function<void(int requestId, const BufferRef& content)> onStdErr)
    : onCreateChannel_(onCreateChannel),
      onUnknownPacket_(onUnknownPacket),
      onAbortRequest_(onAbortRequest),
      onStdErr_(onStdErr) {
}

void Parser::reset() {
  streams_.clear();
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
  TRACE("Parser.beginRequest");
  StreamState& stream = streams_[record->requestId()];
  stream.totalBytesReceived += record->size();
  stream.listener = onCreateChannel_(record->requestId());
}

void Parser::streamParams(const fastcgi::Record* record) {
  TRACE("Parser.streamParams: size=%zu", record->contentLength());
  StreamState& stream = streams_[record->requestId()];
  stream.totalBytesReceived += record->size();

  if (record->contentLength()) {
    stream.processParams(record->content(), record->contentLength());
    return;
  }

  TRACE("Parser.streamParams: fully received!");
  stream.paramsFullyReceived = true;

  BufferRef method;
  BufferRef entity;
  HttpVersion version = HttpVersion::VERSION_1_0;

  if (!stream.listener->onMessageBegin(method, entity, version))
    return;

  for (const auto& header: stream.headers) {
    if (!stream.listener->onMessageHeader(
        BufferRef(header.name()),
        BufferRef(header.value()))) {
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

  if (onStdErr_) {
    BufferRef content(record->content(), record->contentLength());
    onStdErr_(record->requestId(), content);
  }
}

void Parser::streamData(const fastcgi::Record* record) {
  StreamState& stream = streams_[record->requestId()];
  stream.totalBytesReceived += record->size();

  // ignore data-stream, as we've no association in HTTP-layer for it.
}

void Parser::abortRequest(const fastcgi::AbortRequestRecord* record) {
  StreamState& stream = streams_[record->requestId()];
  stream.totalBytesReceived += record->size();

  onAbortRequest_(record->requestId());

  removeStreamState(record->requestId());
}

void Parser::onMessageHeaderEnd(StreamState& stream) {
  TRACE("Parser.onMessageHeaderEnd");
}

} // namespace fastcgi
} // namespace http
} // namespace cortex
