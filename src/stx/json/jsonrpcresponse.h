/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _STX_JSON_JSONRPCRESPONSE_H
#define _STX_JSON_JSONRPCRESPONSE_H
#include <stdlib.h>
#include <functional>
#include <string>
#include "stx/io/outputstream.h"
#include "stx/json/jsonoutputstream.h"

namespace stx {
namespace json {

class JSONRPCResponse {
public:

  static const int kJSONRPCParserError = -32700;
  static const int kJSONRPCPInvalidRequestError = -32600;
  static const int kJSONRPCPMethodNotFoundError = -32601;
  static const int kJSONRPCPInvalidParamsError = -32602;
  static const int kJSONRPCPInternalError = -32603;

  JSONRPCResponse(JSONOutputStream&& output);

  void error(int code, const std::string& message);
  void success(std::function<void (JSONOutputStream* output)> func);

  template <typename T>
  void successAndReturn(const T& ret_val);

  void setID(const std::string& id);

protected:
  std::string id_;
  JSONOutputStream output_;
};

} // namespace json
} // namespace stx

#include "jsonrpcresponse_impl.h"
#endif
