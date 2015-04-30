#include <initializer_list>
#include <vector>
#include <gtest/gtest.h>
#include <cortex-http/fastcgi/Parser.h>
#include <cortex-http/mock/MockInput.h>
#include <cortex-http/HttpRequest.h>
#include <cortex-http/HttpResponse.h>
#include <cortex-http/HttpChannel.h>

using namespace cortex;

class TestListener : public HttpListener { // {{{
 public:
  bool onMessageBegin(const BufferRef& method, const BufferRef& entity,
                      HttpVersion version) override;
  bool onMessageBegin(HttpVersion version, HttpStatus code,
                      const BufferRef& text) override;
  bool onMessageBegin() override;
  bool onMessageHeader(const BufferRef& name, const BufferRef& value) override;
  bool onMessageHeaderEnd() override;
  bool onMessageContent(const BufferRef& chunk) override;
  bool onMessageEnd() override;
  void onProtocolError(HttpStatus code, const std::string& message) override;

 public:
  BufferRef method;
  BufferRef entity;
  HttpVersion version = HttpVersion::VERSION_0_9;
  HttpStatus status = HttpStatus::Undefined;
  BufferRef text;
  int requestMessageBeginCount = 0;
  int genericMessageBeginCount = 0;
  int responseMessageBeginCount = 0;
  HeaderFieldList headers;
  int headersEnd = 0;
  int messageEnd = 0;
  int protocolErrors = 0;
  Buffer body;
};

bool TestListener::onMessageBegin(const BufferRef& method, const BufferRef& entity,
                    HttpVersion version) {
  this->method = method;
  this->entity = entity;
  this->version = version;
  this->requestMessageBeginCount++;

  return true;
}

bool TestListener::onMessageBegin(HttpVersion version, HttpStatus code,
                    const BufferRef& text) {
  this->version = version;
  this->status = code;
  this->text = text;
  this->responseMessageBeginCount++;

  return true;
}

bool TestListener::onMessageBegin() {
  this->genericMessageBeginCount++;

  return true;
}

bool TestListener::onMessageHeader(const BufferRef& name, const BufferRef& value) {
  this->headers.push_back(name.str(), value.str());
  return true;
}

bool TestListener::onMessageHeaderEnd() {
  this->headersEnd++;
  return true;
}

bool TestListener::onMessageContent(const BufferRef& chunk) {
  this->body.push_back(chunk);
  return true;
}

bool TestListener::onMessageEnd() {
  this->messageEnd++;
  return true;
}

void TestListener::onProtocolError(HttpStatus code, const std::string& message) {
  this->protocolErrors++;
}
// }}}

Buffer makeParams(const std::list<std::pair<std::string, std::string>>& params) {
  http::fastcgi::CgiParamStreamWriter w;
  for (const auto param: params)
    w.encode(param.first, param.second);

  Buffer result;
  result.reserve(2 * sizeof(http::fastcgi::Record) + w.output().size());

  // append Params record
  new(result.data()) http::fastcgi::Record(
      http::fastcgi::Type::Params, 42, w.output().size(), 0);
  result.resize(sizeof(http::fastcgi::Record));
  result.push_back(w.output());

  // append Params-EOS
  new(result.data() + result.size()) http::fastcgi::Record(
      http::fastcgi::Type::Params, 42, 0, 0);
  result.resize(result.size() + sizeof(http::fastcgi::Record));

  return result;
}

TEST(http_fastcgi_Parser, simpleRequest) {
  // Tests the following example from the FastCGI spec:
  //
  // {FCGI_BEGIN_REQUEST,   1, {FCGI_RESPONDER, 0}}
  // {FCGI_PARAMS,          1, "\013\002SERVER_PORT80\013\016SER"}
  // {FCGI_PARAMS,          1, "VER_ADDR199.170.183.42 ... "}
  // {FCGI_PARAMS,          1, ""}
  // {FCGI_STDIN,           1, "quantity=100&item=3047936"}
  // {FCGI_STDIN,           1, ""}

  int parsedRequestId = -1;
  std::vector<std::pair<std::string, std::string>> parsedHeaders;

  TestListener httpListener;
  auto onCreateChannel = [&](int requestId) -> HttpListener* {
                            parsedRequestId = requestId;
                            return &httpListener; };
  auto onUnknownPacket = [&](int requestId, int recordId) { };
  auto onAbortRequest = [&](int requestId) { };
  auto onStdErr = [&](int requestId, const BufferRef& content) { };

  http::fastcgi::Parser p(onCreateChannel, onUnknownPacket, onAbortRequest,
                          onStdErr);

  size_t n = p.parseFragment<http::fastcgi::BeginRequestRecord>(
      http::fastcgi::Role::Responder, 42, false);
  EXPECT_EQ(sizeof(http::fastcgi::BeginRequestRecord), n);
  EXPECT_EQ(42, parsedRequestId);

  Buffer fcgistream = makeParams({
      {"SERVER_PORT", "80"},
      {"SERVER_PROTOCOL", "1.1"},
      {"SERVER_NAME", "www.example.com"},
      {"REQUEST_METHOD", "GET"},
      {"REQUEST_URI", "/index.html"},
      {"HTTP_USER_AGENT", "cortex-test"},
      {"HTTP_CONTENT_TYPE", "text/plain"},
  });
  n = p.parseFragment(fcgistream);

  EXPECT_EQ(fcgistream.size(), n);
  EXPECT_EQ(0, httpListener.protocolErrors);

  // CONTENT
  constexpr BufferRef content = "quantity=100&item=3047936";
  fcgistream.reserve(sizeof(http::fastcgi::Record) + content.size());
  new(fcgistream.data()) http::fastcgi::Record(
      http::fastcgi::Type::StdIn, 42, content.size(), 0);
  fcgistream.resize(sizeof(http::fastcgi::Record));
  fcgistream.push_back(content);
  n = p.parseFragment(fcgistream);

  EXPECT_EQ(fcgistream.size(), n);
  EXPECT_EQ(0, httpListener.protocolErrors);
  EXPECT_EQ(content, httpListener.body);

  // content-EOS
  new(fcgistream.data()) http::fastcgi::Record(
      http::fastcgi::Type::StdIn, 42, 0, 0);
  fcgistream.resize(sizeof(http::fastcgi::Record));
  n = p.parseFragment(fcgistream);

  EXPECT_EQ(fcgistream.size(), n);
  EXPECT_EQ("cortex-test", httpListener.headers.get("User-Agent"));
  EXPECT_EQ("text/plain", httpListener.headers.get("Content-Type"));
  EXPECT_EQ(1, httpListener.requestMessageBeginCount);
  EXPECT_EQ(1, httpListener.headersEnd);
  EXPECT_EQ(1, httpListener.messageEnd);
  EXPECT_EQ(0, httpListener.protocolErrors);
}

class ResponseParser : public http::fastcgi::Parser {
 public:
  ResponseParser(
      std::function<HttpListener*(int requestId)> onCreateChannel,
      std::function<void(int requestId, int recordId)> onUnknownPacket,
      std::function<void(int requestId)> onAbortRequest,
      std::function<void(int requestId, const BufferRef&)> onStdErr);
};

TEST(http_fastcgi_Parser, simpleResponse) {
  // Tests the following example from the FastCGI spec:
  //
  // {FCGI_STDOUT,      1, "Content-type: text/html\r\n\r\n<html>\n<head> ... "}
  // {FCGI_STDOUT,      1, ""}
  // {FCGI_END_REQUEST, 1, {0, FCGI_REQUEST_COMPLETE}}

  // --------------------------------------------------------------------------
  // generate response bitstream

  constexpr BufferRef content =
      "Content-Type: text/html\r\n"
      "\r\n"
      "<html>\n<head> ...";

  Buffer fcgistream;
  fcgistream.reserve(1024);

  // StdOut-header with payload
  new(fcgistream.data()) http::fastcgi::Record(
      http::fastcgi::Type::StdOut, 1, content.size(), 0);
  fcgistream.resize(sizeof(http::fastcgi::Record));
  fcgistream.push_back(content);

  // StdOut-header (EOS)
  new(fcgistream.end()) http::fastcgi::Record(
      http::fastcgi::Type::StdOut, 1, 0, 0);
  fcgistream.resize(fcgistream.size() + sizeof(http::fastcgi::Record));

  // EndRequest-header
  new(fcgistream.end()) http::fastcgi::EndRequestRecord(
      1, // requestId
      0, // appStatus
      http::fastcgi::ProtocolStatus::RequestComplete);
  fcgistream.resize(fcgistream.size() + sizeof(http::fastcgi::EndRequestRecord));

  // --------------------------------------------------------------------------
  // parse the response bitstream

  int parsedRequestId = -1;
  TestListener httpListener;
  auto onCreateChannel = [&](int requestId) -> HttpListener* {
                            parsedRequestId = requestId;
                            return &httpListener; };
  auto onUnknownPacket = [&](int requestId, int recordId) { };
  auto onAbortRequest = [&](int requestId) { };
  auto onStdErr = [&](int requestId, const BufferRef& content) { };

  http::fastcgi::Parser parser(
      onCreateChannel, onUnknownPacket, onAbortRequest, onStdErr);

  size_t n = parser.parseFragment(fcgistream);

  EXPECT_EQ(fcgistream.size(), n);
  EXPECT_EQ(0, httpListener.protocolErrors);
  EXPECT_EQ(1, parsedRequestId);
  EXPECT_EQ(content, httpListener.body);
}
