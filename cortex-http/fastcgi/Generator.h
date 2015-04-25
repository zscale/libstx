// This file is part of the "libcortex" project
//   (c) 2009-2015 Christian Parpart <https://github.com/christianparpart>
//   (c) 2014-2015 Paul Asmuth <https://github.com/paulasmuth>
//
// libcortex is free software: you can redistribute it and/or modify it under
// the terms of the GNU Affero General Public License v3.0.
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <cortex-http/Api.h>
#include <cortex-http/HeaderFieldList.h>
#include <cortex-http/HttpInfo.h>
#include <cortex-http/HttpRequestInfo.h>
#include <cortex-http/HttpResponseInfo.h>
#include <cortex-base/io/FileRef.h>
#include <cortex-base/Buffer.h>
#include <cortex-base/sysconfig.h>

namespace cortex {

class HttpDateGenerator;
class EndPointWriter;

namespace http {
namespace fastcgi {

class CORTEX_HTTP_API Generator {
 public:
  Generator(
      int requestId,
      HttpDateGenerator* dateGenerator,
      EndPointWriter* writer);

  void generateRequest(const HttpRequestInfo& info, Buffer&& chunk);
  void generateRequest(const HttpRequestInfo& info, const BufferRef& chunk);
  void generateResponse(const HttpResponseInfo& info, const BufferRef& chunk);
  void generateResponse(const HttpResponseInfo& info, Buffer&& chunk);
  void generateResponse(const HttpResponseInfo& info, FileRef&& chunk);
  void generateBody(Buffer&& chunk);
  void generateBody(const BufferRef& chunk);
  void generateBody(FileRef&& chunk);
  void generateTrailer(const HeaderFieldList& trailers);

 private:
  void generateRequestLine(const HttpRequestInfo& info);
  void generateResponseLine(const HttpResponseInfo& info);
  void generateHeaders(const HttpInfo& info);
  void generateResponseInfo(const HttpResponseInfo& info);
  void flushBuffer();

 private:
  int requestId_;
  size_t bytesTransmitted_;
  HttpDateGenerator* dateGenerator_;
  size_t contentLength_;
  Buffer buffer_;
  EndPointWriter* writer_;
};

} // namespace fastcgi
} // namespace http
} // namespace cortex
