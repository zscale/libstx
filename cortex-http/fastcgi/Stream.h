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
#include <cortex-http/fastcgi/RequestParser.h>
#include <cortex-http/fastcgi/Generator.h>
#include <cortex-http/HttpTransport.h>
#include <cortex-http/HttpHandler.h>
#include <cortex-base/Buffer.h>
#include <cortex-base/TimeSpan.h>
#include <memory>
#include <unordered_map>

namespace cortex {

class HttpDateGenerator;
class HttpOutputCompressor;
class HttpChannel;
class HttpListener;
class EndPointWriter;

namespace http {
namespace fastcgi {

/**
 * FastCGI request stream.
 *
 * Represents a single request stream inside a (multiplexed) FastCGI connection.
 */
struct Stream : public CgiParamStreamReader {
  Stream(int rid, EndPointWriter* w, HttpChannel* channel);

  int requestId;
  HttpChannel* channel;
  Generator generator;
  CompletionHandler onComplete; // app-completion handler

  // input
  std::vector<std::pair<std::string, std::string>> environment;
  size_t requestContentActualLength;
  size_t requestContentExpectedLength; // CONTENT_LENGTH env value
  bool paramsComplete; //!< parameters fully received
  bool aborted; //!< request got aborted (downstream side)

 private:
  void onParam(const char *name, size_t nameLen,
               const char *value, size_t valueLen) override;
};

} // namespace fastcgi
} // namespace http
} // namespace cortex
