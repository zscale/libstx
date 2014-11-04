// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero/Api.h>
#include <xzero/Buffer.h>
#include <xzero/http/HeaderFieldList.h>
#include <xzero/http/HttpVersion.h>
#include <xzero/http/HttpMethod.h>
#include <xzero/http/HttpInput.h>
#include <memory>

namespace xzero {

/**
 * Represents an HTTP request message.
 */
class XZERO_API HttpRequest {
 public:
  HttpRequest();
  explicit HttpRequest(std::unique_ptr<HttpInput>&& input);
  HttpRequest(const std::string& method, const std::string& path,
              HttpVersion version, bool secure, const HeaderFieldList& headers,
              std::unique_ptr<HttpInput>&& input);


  HttpMethod method() const noexcept { return method_; }
  const std::string& unparsedMethod() const noexcept { return unparsedMethod_; }
  void setMethod(const std::string& value);

  bool setUri(const std::string& uri);
  const std::string& unparsedUri() const noexcept { return path_; }
  const std::string& path() const noexcept { return path_; }
  const std::string& query() const noexcept { return query_; }
  int directoryDepth() const noexcept { return directoryDepth_; }

  HttpVersion version() const noexcept { return version_; }
  void setVersion(HttpVersion version) { version_ = version; }

  const HeaderFieldList& headers() const noexcept { return headers_; }
  HeaderFieldList& headers() { return headers_; }

  const std::string& host() const noexcept { return host_; }
  void setHost(const std::string& value);

  bool isSecure() const noexcept { return secure_; }
  void setSecure(bool secured) { secure_ = secured; }

  HttpInput* input() const { return input_.get(); }
  void setInput(std::unique_ptr<HttpInput>&& input) { input_ = std::move(input); }

  bool expect100Continue() const noexcept { return expect100Continue_; }
  void setExpect100Continue(bool value) noexcept { expect100Continue_ = value; }

  void recycle();

 private:
  HttpMethod method_;
  std::string unparsedMethod_;

  std::string unparsedUri_;
  std::string path_;
  std::string query_;
  int directoryDepth_;

  HttpVersion version_;

  bool secure_;
  bool expect100Continue_;
  std::string host_;

  HeaderFieldList headers_;

  std::unique_ptr<HttpInput> input_;

  bool handled_;
};

}  // namespace xzero
