#pragma once

#include <xzero/Api.h>
#include <xzero/Buffer.h>
#include <xzero/http/HeaderFieldList.h>
#include <xzero/http/HttpVersion.h>
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

  const std::string& method() const { return method_; }
  void setMethod(const std::string& value) { method_ = value; }

  const std::string& path() const { return path_; }
  void setPath(const std::string& value) { path_ = value; }

  HttpVersion version() const { return version_; }
  void setVersion(HttpVersion version) { version_ = version; }

  const HeaderFieldList& headers() const { return headers_; }
  HeaderFieldList& headers() { return headers_; }

  bool isSecure() const { return secure_; }
  void setSecure(bool secured) { secure_ = secured; }

  HttpInput* input() const { return input_.get(); }
  void setInput(std::unique_ptr<HttpInput>&& input) { input_ = std::move(input); }

  bool expect100Continue() const noexcept { return expect100Continue_; }
  void setExpect100Continue(bool value) noexcept { expect100Continue_ = value; }

  void recycle();

 private:
  std::string method_;
  std::string path_;
  HttpVersion version_;
  bool secure_;
  bool expect100Continue_;
  HeaderFieldList headers_;

  std::unique_ptr<HttpInput> input_;
  bool handled_;
};

}  // namespace xzero
