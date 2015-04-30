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
      totalBytesReceived(0),
      paramsFullyReceived(false),
      contentFullyReceived(false),
      params(),
      headers(),
      body() {
}

Parser::StreamState::~StreamState() {
}

void Parser::StreamState::reset() {
  listener = nullptr;
  totalBytesReceived = 0;
  paramsFullyReceived = false;
  contentFullyReceived = false;
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
  // SERVER_NAME (aka. Host request header)
  //
  // REMOTE_ADDR
  // REMOTE_PORT
  // REMOTE_IDENT
  //
  // HTTPS
  //
  // REQUEST_METHOD
  // REQUEST_URI
  //
  // AUTH_TYPE
  // QUERY_STRING
  //
  // DOCUMENT_ROOT
  // PATH_TRANSLATED
  // PATH_INFO
  // SCRIPT_FILENAME
  //
  // CONTENT_TYPE
  // CONTENT_LENGTH
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

Parser::StreamState& Parser::getStream(int requestId) {
  auto s = streams_.find(requestId);
  if (s != streams_.end())
    return s->second;

  StreamState& stream = streams_[requestId];
  stream.listener = onCreateChannel_(requestId);
  return stream;
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
      onUnknownPacket_(record->requestId(), (int) record->type());
      ;//write<fastcgi::UnknownTypeRecord>(record->type(), record->requestId());
  }
}

void Parser::beginRequest(const fastcgi::BeginRequestRecord* record) {
  TRACE("Parser.beginRequest");
  StreamState& stream = getStream(record->requestId());
  stream.totalBytesReceived += record->size();
}

void Parser::streamParams(const fastcgi::Record* record) {
  TRACE("Parser.streamParams: size=%zu", record->contentLength());
  StreamState& stream = getStream(record->requestId());
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
    stream.listener->onMessageHeader(BufferRef(header.name()),
                                     BufferRef(header.value()));
  }

  if (!stream.listener->onMessageHeaderEnd()) {
    TRACE("Parser.streamParams: onMessageHeaderEnd returned false. Bailing out.");
    return;
  }

  if (!stream.body.empty()) {
    TRACE("Parser.streamParams: onMessageContent");
    stream.listener->onMessageContent(stream.body);
  }

  if (stream.paramsFullyReceived && stream.contentFullyReceived) {
    TRACE("Parser.streamParams: onMessageEnd");
    stream.listener->onMessageEnd();
  }
}

void Parser::streamStdIn(const fastcgi::Record* record) {
  StreamState& stream = getStream(record->requestId());
  stream.totalBytesReceived += record->size();

  TRACE("Parser.streamStdIn: payload:%zu, paramsEOS:%s",
      record->contentLength(),
      stream.paramsFullyReceived ? "true" : "false");

  BufferRef content(record->content(), record->contentLength());
  stream.listener->onMessageContent(content);

  if (record->contentLength() == 0)
    stream.contentFullyReceived = true;

  if (stream.paramsFullyReceived && stream.contentFullyReceived) {
    TRACE("Parser.streamStdIn: onMessageEnd");
    stream.listener->onMessageEnd();
  }
}

void Parser::streamStdOut(const fastcgi::Record* record) {
  TRACE("Parser.streamStdOut: %zu", record->contentLength());

  StreamState& stream = getStream(record->requestId());
  stream.totalBytesReceived += record->size();

  if (record->contentLength() == 0)
    stream.contentFullyReceived = true;

  BufferRef content(record->content(), record->contentLength());
  stream.listener->onMessageContent(content);

  if (stream.contentFullyReceived) {
    TRACE("Parser.streamStdOut: onMessageEnd");
    stream.listener->onMessageEnd();
  }
}

void Parser::streamStdErr(const fastcgi::Record* record) {
  TRACE("Parser.streamStdErr: %zu", record->contentLength());

  StreamState& stream = getStream(record->requestId());
  stream.totalBytesReceived += record->size();

  if (onStdErr_) {
    BufferRef content(record->content(), record->contentLength());
    onStdErr_(record->requestId(), content);
  }
}

void Parser::streamData(const fastcgi::Record* record) {
  TRACE("Parser.streamData: %zu", record->contentLength());

  StreamState& stream = getStream(record->requestId());
  stream.totalBytesReceived += record->size();

  // ignore data-stream, as we've no association in HTTP-layer for it.
}

void Parser::abortRequest(const fastcgi::AbortRequestRecord* record) {
  TRACE("Parser.abortRequest");

  StreamState& stream = getStream(record->requestId());
  stream.totalBytesReceived += record->size();

  onAbortRequest_(record->requestId());

  removeStreamState(record->requestId());
}

} // namespace fastcgi
} // namespace http
} // namespace cortex
