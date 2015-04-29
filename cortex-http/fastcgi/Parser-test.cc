#include <initializer_list>
#include <vector>
#include <gtest/gtest.h>
#include <cortex-http/fastcgi/Parser.h>
#include <cortex-http/mock/MockInput.h>
#include <cortex-http/HttpRequest.h>
#include <cortex-http/HttpResponse.h>
#include <cortex-http/HttpChannel.h>
#include <cortex-base/executor/DirectExecutor.h>

using namespace cortex;

Buffer makeParams(const std::list<std::pair<std::string, std::string>>& params) {
  Buffer result;

  http::fastcgi::CgiParamStreamWriter w;
  for (const auto param: params) {
    printf("encoding param <%s, %s>\n", param.first.c_str(), param.second.c_str());
    w.encode(param.first, param.second);
  }
  printf("param stream size: %zu\n", w.output().size());

  result.reserve(sizeof(http::fastcgi::Record) + w.output().size());
  printf("param record size: %zu\n", result.capacity());

  new(result.data()) http::fastcgi::Record(
      http::fastcgi::Type::Params, 42, w.output().size(), 0);
  result.resize(sizeof(http::fastcgi::Record));
  result.push_back(w.output());

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
  Buffer parsedContent;
  std::vector<std::pair<std::string, std::string>> parsedHeaders;

  DirectExecutor executor;
  auto handler = [&](cortex::HttpRequest* req, cortex::HttpResponse* resp) { };
  auto onCreateChannel = [&](int requestId) -> HttpListener* {
    parsedRequestId = requestId;
    return new HttpChannel(
        nullptr, // transport
        handler,
        new MockInput(),
        1024, // maxRequestUriLength,
        1024, // maxRequestBodyLength
        nullptr); // outputCompressor
  };
  auto onUnknownPacket = [&](int requestId, int recordId) { };
  auto onAbortRequest = [&](int requestId) { };

  http::fastcgi::Parser p(onCreateChannel, onUnknownPacket, onAbortRequest);

  size_t n = p.parseFragment<http::fastcgi::BeginRequestRecord>(
      http::fastcgi::Role::Responder, 42, false);
  ASSERT_EQ(sizeof(http::fastcgi::BeginRequestRecord), n);
  ASSERT_EQ(42, parsedRequestId);

  Buffer params = makeParams({
      {"SERVER_PORT", "80"},
      {"SERVER_PROTOCOL", "1.1"},
      {"SERVER_NAME", "www.example.com"},
      {"HTTP_USER_AGENT", "cortex-test"},
  });
  n = p.parseFragment(params);
  printf("parsed params bytes: %zu\n", n);
  ASSERT_EQ(params.size(), n);

  // CONTENT
  constexpr BufferRef content = "quantity=100&item=3047936";
  char data[sizeof(http::fastcgi::Record) + content.size()];
  new(data) http::fastcgi::Record(
      http::fastcgi::Type::StdIn, 42, content.size(), 0);
  memcpy(data + sizeof(http::fastcgi::Record), content.data(), content.size());
  BufferRef pdata(data, ((const http::fastcgi::Record*)data)->size());
  n = p.parseFragment(pdata);

  ASSERT_EQ(pdata.size(), n);
  ASSERT_EQ(content, parsedContent);

  // EOS
  new(data) http::fastcgi::Record(
      http::fastcgi::Type::StdIn, 42, 0, 0);
  pdata = BufferRef(data, ((const http::fastcgi::Record*)data)->size());
  n = p.parseFragment(pdata);

  ASSERT_EQ(pdata.size(), n);
}
