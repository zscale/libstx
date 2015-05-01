// This file is part of the "libcortex" project
//   (c) 2009-2015 Christian Parpart <https://github.com/christianparpart>
//   (c) 2014-2015 Paul Asmuth <https://github.com/paulasmuth>
//
// libcortex is free software: you can redistribute it and/or modify it under
// the terms of the GNU Affero General Public License v3.0.
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <cortex-http/fastcgi/Generator.h>
#include <cortex-base/net/EndPointWriter.h>
#include <cortex-base/net/ByteArrayEndPoint.h>
#include <gtest/gtest.h>

using namespace cortex;

#if 1 == 0
TEST(http_fastcgi_Generator, simpleResponse) {
  EndPointWriter writer;

  constexpr BufferRef content = "hello, world";
  HeaderFieldList headers = {
    {"Foo", "the-foo"},
    {"Bar", "the-bar"}
  };
  HeaderFieldList trailers;
  HttpResponseInfo info(HttpVersion::VERSION_1_1, HttpStatus::Ok, "my",
      false, content.size(), headers, trailers);

  http::fastcgi::Generator generator(1, nullptr, &writer);
  generator.generateResponse(info);
  generator.generateBody(content);
  generator.generateTrailer(trailers);

  ByteArrayEndPoint ep(nullptr);
  writer.flush(&ep);

  //...
}
#endif
