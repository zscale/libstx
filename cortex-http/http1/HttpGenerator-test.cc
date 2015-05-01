// This file is part of the "x0" project, http://cortex.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

// HTTP semantic tests

#include <cortex-http/HttpRequestInfo.h>
#include <cortex-http/HttpResponseInfo.h>
#include <cortex-http/http1/HttpGenerator.h>
#include <cortex-base/net/EndPointWriter.h>
#include <cortex-base/net/ByteArrayEndPoint.h>
#include <cortex-base/Buffer.h>
#include <gtest/gtest.h>

using namespace cortex;

// TEST(HttpGenerator, recycle) {
// }
//
// TEST(HttpGenerator, generateRequest) {
// }

// XXX No headers, no body.
TEST(HttpGenerator, generateResponse_empty) {
  EndPointWriter writer;
  http1::HttpGenerator generator(&writer);

  HttpResponseInfo info(HttpVersion::VERSION_1_1, HttpStatus::Ok, "my",
                        false, 0, {}, {});

  generator.generateResponse(info, BufferRef());
  generator.generateTrailer({});

  ByteArrayEndPoint ep(nullptr);
  writer.flush(&ep);

  ASSERT_EQ("HTTP/1.1 200 my\r\nContent-Length: 0\r\n\r\n", ep.output());
}

// XXX some headers, no body.
TEST(HttpGenerator, generateResponse_headers) {
  EndPointWriter writer;
  http1::HttpGenerator generator(&writer);

  HeaderFieldList headers = {
    {"Foo", "the-foo"},
    {"Bar", "the-bar"}
  };
  HeaderFieldList trailers;
  HttpResponseInfo info(HttpVersion::VERSION_1_1, HttpStatus::Ok, "my",
                        false, 0, headers, trailers);

  generator.generateResponse(info, BufferRef());
  generator.generateTrailer(trailers);

  ByteArrayEndPoint ep(nullptr);
  writer.flush(&ep);

  ASSERT_EQ("HTTP/1.1 200 my\r\nFoo: the-foo\r\nBar: the-bar\r\nContent-Length: 0\r\n\r\n", ep.output());
}

// XXX no headers, static (fixed-size) body.
TEST(HttpGenerator, generateResponse_static_body) {
  EndPointWriter writer;
  http1::HttpGenerator generator(&writer);

  HeaderFieldList headers;
  HeaderFieldList trailers;
  BufferRef body = "body";
  HttpResponseInfo info(HttpVersion::VERSION_1_1, HttpStatus::Ok, "my",
                        false, body.size(), headers, trailers);

  generator.generateResponse(info, body);
  generator.generateTrailer(trailers);

  ByteArrayEndPoint ep(nullptr);
  writer.flush(&ep);

  ASSERT_EQ("HTTP/1.1 200 my\r\nContent-Length: 4\r\n\r\nbody", ep.output());
}

// XXX no headers, dynamic (chunked) body
TEST(HttpGenerator, generateResponse_chunked) {
  EndPointWriter writer;
  http1::HttpGenerator generator(&writer);

  HeaderFieldList headers;
  HeaderFieldList trailers;
  HttpResponseInfo info(HttpVersion::VERSION_1_1, HttpStatus::Ok, "my",
                        false, Buffer::npos, headers, trailers);

  BufferRef body = "body";
  generator.generateResponse(info, body);
  generator.generateTrailer(trailers);

  ByteArrayEndPoint ep(nullptr);
  writer.flush(&ep);

  ASSERT_EQ("HTTP/1.1 200 my\r\nTransfer-Encoding: chunked\r\n\r\n4\r\nbody\r\n0\r\n\r\n", ep.output());
}

// XXX no headers, dynamic (chunked) body with trailers
TEST(HttpGenerator, generateResponse_chunked_trailer) {
  EndPointWriter writer;
  http1::HttpGenerator generator(&writer);

  HeaderFieldList headers;
  HeaderFieldList trailers = {
    {"Foo", "the-foo"},
    {"Bar", "the-bar"},
  };
  BufferRef body = "body";
  HttpResponseInfo info(HttpVersion::VERSION_1_1, HttpStatus::Ok, "my",
                        false, Buffer::npos, headers, trailers);

  generator.generateResponse(info, body);
  generator.generateTrailer(trailers);

  ByteArrayEndPoint ep(nullptr);
  writer.flush(&ep);

  ASSERT_EQ("HTTP/1.1 200 my\r\nTrailer: Foo, Bar\r\nTransfer-Encoding: chunked\r\n\r\n4\r\nbody\r\n0\r\nFoo: the-foo\r\nBar: the-bar\r\n\r\n", ep.output());
}

// XXX no headers, static (fixed-size) body with trailers (triggers chunking)
TEST(HttpGenerator, generateResponse_chunked_trailer2) {
  EndPointWriter writer;
  http1::HttpGenerator generator(&writer);

  HeaderFieldList headers;
  HeaderFieldList trailers = {
    {"Foo", "the-foo"},
    {"Bar", "the-bar"},
  };
  BufferRef body = "body";
  HttpResponseInfo info(HttpVersion::VERSION_1_1, HttpStatus::Ok, "my",
                        false, body.size(), headers, trailers);

  generator.generateResponse(info, body);
  generator.generateTrailer(trailers);

  ByteArrayEndPoint ep(nullptr);
  writer.flush(&ep);

  ASSERT_EQ("HTTP/1.1 200 my\r\nTrailer: Foo, Bar\r\nTransfer-Encoding: chunked\r\n\r\n4\r\nbody\r\n0\r\nFoo: the-foo\r\nBar: the-bar\r\n\r\n", ep.output());
}

// TEST(HttpGenerator, generateBody_Buffer) {
// }
// TEST(HttpGenerator, generateBody_BufferRef) {
// }
// TEST(HttpGenerator, generateBody_FileRef) {
// }
