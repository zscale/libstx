// This file is part of the "x0" project, http://cortex.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <cortex-http/http1/HttpParser.h>
#include <cortex-http/HttpListener.h>
#include <cortex-http/HttpStatus.h>
#include <cortex-base/RuntimeError.h>
#include <cortex-base/Buffer.h>
#include <vector>
#include <gtest/gtest.h>

using namespace cortex;
using namespace cortex::http;
using namespace cortex::http::http1;

class HttpParserListener : public HttpListener {  // {{{
 public:
  HttpParserListener();

  void onMessageBegin(const BufferRef& method, const BufferRef& entity,
                      HttpVersion version) override;
  void onMessageBegin(HttpVersion version, HttpStatus code,
                      const BufferRef& text) override;
  void onMessageBegin() override;
  void onMessageHeader(const BufferRef& name, const BufferRef& value) override;
  void onMessageHeaderEnd() override;
  void onMessageContent(const BufferRef& chunk) override;
  void onMessageEnd() override;
  void onProtocolError(HttpStatus code, const std::string& msg) override;

 public:
  std::string method;
  std::string entity;
  HttpVersion version;
  HttpStatus statusCode;
  std::string statusReason;
  std::vector<std::pair<std::string, std::string>> headers;
  Buffer body;
  HttpStatus errorCode;
  std::string errorMessage;

  bool messageBegin;
  bool headerEnd;
  bool messageEnd;
};

HttpParserListener::HttpParserListener()
    : method(),
      entity(),
      version(HttpVersion::UNKNOWN),
      statusCode(HttpStatus::Undefined),
      statusReason(),
      headers(),
      errorCode(HttpStatus::Undefined),
      errorMessage(),
      messageBegin(false),
      headerEnd(false),
      messageEnd(false) {
}

void HttpParserListener::onMessageBegin(const BufferRef& method,
                                        const BufferRef& entity,
                                        HttpVersion version) {
  this->method = method.str();
  this->entity = entity.str();
  this->version = version;
}

void HttpParserListener::onMessageBegin(HttpVersion version, HttpStatus code,
                                        const BufferRef& text) {
  this->version = version;
  this->statusCode = code;
  this->statusReason = text.str();
}

void HttpParserListener::onMessageBegin() {
  messageBegin = true;
}

void HttpParserListener::onMessageHeader(const BufferRef& name,
                                         const BufferRef& value) {
  headers.push_back(std::make_pair(name.str(), value.str()));
}

void HttpParserListener::onMessageHeaderEnd() {
  headerEnd = true;
}

void HttpParserListener::onMessageContent(const BufferRef& chunk) {
  body += chunk;
}

void HttpParserListener::onMessageEnd() {
  messageEnd = true;
}

void HttpParserListener::onProtocolError(HttpStatus code,
                                         const std::string& msg) {
  errorCode = code;
  errorMessage = msg;
}
// }}}

TEST(HttpParser, requestLine0) {
  /* Seems like in HTTP/0.9 it was possible to create
   * very simple request messages.
   */
  HttpParserListener listener;
  HttpParser parser(HttpParser::REQUEST, &listener);
  parser.parseFragment("GET /\r\n");

  ASSERT_EQ("GET", listener.method);
  ASSERT_EQ("/", listener.entity);
  ASSERT_EQ(HttpVersion::VERSION_0_9, listener.version);
  ASSERT_TRUE(listener.headerEnd);
  ASSERT_TRUE(listener.messageEnd);
  ASSERT_EQ(0, listener.headers.size());
  ASSERT_EQ(0, listener.body.size());
}

TEST(HttpParser, requestLine1) {
  HttpParserListener listener;
  HttpParser parser(HttpParser::REQUEST, &listener);
  parser.parseFragment("GET / HTTP/0.9\r\n\r\n");

  ASSERT_EQ("GET", listener.method);
  ASSERT_EQ("/", listener.entity);
  ASSERT_EQ(HttpVersion::VERSION_0_9, listener.version);
  ASSERT_EQ(0, listener.headers.size());
  ASSERT_EQ(0, listener.body.size());
}

TEST(HttpParser, requestLine2) {
  HttpParserListener listener;
  HttpParser parser(HttpParser::REQUEST, &listener);
  parser.parseFragment("HEAD /foo?bar HTTP/1.0\r\n\r\n");

  ASSERT_EQ("HEAD", listener.method);
  ASSERT_EQ("/foo?bar", listener.entity);
  ASSERT_EQ(HttpVersion::VERSION_1_0, listener.version);
  ASSERT_EQ(0, listener.headers.size());
  ASSERT_EQ(0, listener.body.size());
}

TEST(HttpParser, requestLine_invalid1_MissingPathAndProtoVersion) {
  HttpParserListener listener;
  HttpParser parser(HttpParser::REQUEST, &listener);
  parser.parseFragment("GET\r\n\r\n");
  ASSERT_EQ(HttpStatus::BadRequest, listener.errorCode);
}

TEST(HttpParser, requestLine_invalid3_InvalidVersion) {
  HttpParserListener listener;
  HttpParser parser(HttpParser::REQUEST, &listener);
  parser.parseFragment("GET / HTTP/0\r\n\r\n");
  ASSERT_EQ((int)HttpStatus::BadRequest, (int)listener.errorCode);
}

TEST(HttpParser, requestLine_invalid3_CharsAfterVersion) {
  HttpParserListener listener;
  HttpParser parser(HttpParser::REQUEST, &listener);
  parser.parseFragment("GET / HTTP/1.1b\r\n\r\n");
  ASSERT_EQ((int)HttpStatus::BadRequest, (int)listener.errorCode);
}

TEST(HttpParser, requestLine_invalid5_SpaceAfterVersion) {
  HttpParserListener listener;
  HttpParser parser(HttpParser::REQUEST, &listener);
  parser.parseFragment("GET / HTTP/1.1 \r\n\r\n");
  ASSERT_EQ((int)HttpStatus::BadRequest, (int)listener.errorCode);
}

TEST(HttpParser, requestLine_invalid6_UnsupportedVersion) {
  HttpParserListener listener;
  HttpParser parser(HttpParser::REQUEST, &listener);

  // Actually, we could make it a ParserError, or HttpClientError or so,
  // But to make googletest lib happy, we should make it even a distinct class.
  ASSERT_THROW(parser.parseFragment("GET / HTTP/1.2\r\n\r\n"), RuntimeError);
}

TEST(HttpParser, headers1) {
  HttpParserListener listener;
  HttpParser parser(HttpParser::MESSAGE, &listener);
  parser.parseFragment(
      "Foo: the foo\r\n"
      "Content-Length: 6\r\n"
      "\r\n"
      "123456");

  ASSERT_EQ("Foo", listener.headers[0].first);
  ASSERT_EQ("the foo", listener.headers[0].second);
  ASSERT_EQ("123456", listener.body);
}

TEST(HttpParser, invalidHeader1) {
  HttpParserListener listener;
  HttpParser parser(HttpParser::MESSAGE, &listener);
  size_t n = parser.parseFragment("Foo : the foo\r\n"
                                  "\r\n");

  ASSERT_EQ(HttpStatus::BadRequest, listener.errorCode);
  ASSERT_EQ(3, n);
  ASSERT_EQ(0, listener.headers.size());
}

TEST(HttpParser, invalidHeader2) {
  HttpParserListener listener;
  HttpParser parser(HttpParser::MESSAGE, &listener);
  size_t n = parser.parseFragment("Foo\r\n"
                                  "\r\n");

  ASSERT_EQ(HttpStatus::BadRequest, listener.errorCode);
  ASSERT_EQ(5, n);
  ASSERT_EQ(0, listener.headers.size());
}

TEST(HttpParser, requestWithHeaders) {
  HttpParserListener listener;
  HttpParser parser(HttpParser::REQUEST, &listener);
  parser.parseFragment(
      "GET / HTTP/0.9\r\n"
      "Foo: the foo\r\n"
      "X-Bar: the bar\r\n"
      "\r\n");

  ASSERT_EQ("GET", listener.method);
  ASSERT_EQ("/", listener.entity);
  ASSERT_EQ(HttpVersion::VERSION_0_9, listener.version);
  ASSERT_EQ(2, listener.headers.size());
  ASSERT_EQ(0, listener.body.size());

  ASSERT_EQ("Foo", listener.headers[0].first);
  ASSERT_EQ("the foo", listener.headers[0].second);

  ASSERT_EQ("X-Bar", listener.headers[1].first);
  ASSERT_EQ("the bar", listener.headers[1].second);
}

TEST(HttpParser, requestWithHeadersAndBody) {
  HttpParserListener listener;
  HttpParser parser(HttpParser::REQUEST, &listener);
  parser.parseFragment(
      "GET / HTTP/0.9\r\n"
      "Foo: the foo\r\n"
      "X-Bar: the bar\r\n"
      "Content-Length: 6\r\n"
      "\r\n"
      "123456");

  ASSERT_EQ("123456", listener.body);
}

// no chunks except the EOS-chunk
TEST(HttpParser, requestWithHeadersAndBodyChunked1) {
  HttpParserListener listener;
  HttpParser parser(HttpParser::REQUEST, &listener);
  parser.parseFragment(
      "GET / HTTP/0.9\r\n"
      "Transfer-Encoding: chunked\r\n"
      "\r\n"
      "0\r\n"
      "\r\n");

  ASSERT_EQ("", listener.body);
}

// exactly one data chunk
TEST(HttpParser, requestWithHeadersAndBodyChunked2) {
  HttpParserListener listener;
  HttpParser parser(HttpParser::REQUEST, &listener);
  parser.parseFragment(
      "GET / HTTP/0.9\r\n"
      "Transfer-Encoding: chunked\r\n"
      "\r\n"

      "6\r\n"
      "123456"
      "\r\n"

      "0\r\n"
      "\r\n");

  ASSERT_EQ("123456", listener.body);
}

// more than one data chunk
TEST(HttpParser, requestWithHeadersAndBodyChunked3) {
  HttpParserListener listener;
  HttpParser parser(HttpParser::REQUEST, &listener);
  parser.parseFragment(
      "GET / HTTP/0.9\r\n"
      "Transfer-Encoding: chunked\r\n"
      "\r\n"

      "6\r\n"
      "123456"
      "\r\n"

      "6\r\n"
      "123456"
      "\r\n"

      "0\r\n"
      "\r\n");

  ASSERT_EQ("123456123456", listener.body);
}

// first chunk is missing CR LR
TEST(HttpParser, requestWithHeadersAndBodyChunked_invalid1) {
  HttpParserListener listener;
  HttpParser parser(HttpParser::REQUEST, &listener);
  size_t n = parser.parseFragment(
      "GET / HTTP/0.9\r\n"
      "Transfer-Encoding: chunked\r\n"
      "\r\n"

      "6\r\n"
      "123456"
      //"\r\n" // should bailout here

      "0\r\n"
      "\r\n");

  ASSERT_EQ(55, n);
  ASSERT_EQ(HttpStatus::BadRequest, listener.errorCode);
}

TEST(HttpParser, pipelined1) {
  HttpParserListener listener;
  HttpParser parser(HttpParser::REQUEST, &listener);
  constexpr BufferRef input = "GET /foo HTTP/1.1\r\n\r\n"
                              "HEAD /bar HTTP/0.9\r\n\r\n";
  size_t n = parser.parseFragment(input);

  EXPECT_EQ("GET", listener.method);
  EXPECT_EQ("/foo", listener.entity);
  EXPECT_EQ(HttpVersion::VERSION_1_1, listener.version);

  parser.parseFragment(input.ref(n));

  EXPECT_EQ("HEAD", listener.method);
  EXPECT_EQ("/bar", listener.entity);
  EXPECT_EQ(HttpVersion::VERSION_0_9, listener.version);
}
