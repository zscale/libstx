// This file is part of the "x0" project
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <xzero-http/Api.h>
#include <xzero-http/HttpFileRepository.h>
#include <xzero-http/HttpMemoryFile.h>
#include <unordered_map>
#include <string>

namespace xzero {

class MimeTypes;

/**
 * In-memory file store for HttpFileHandler.
 *
 * @see HttpLocalFileRepository
 * @see HttpFileHandler
 * @see HttpMemoryFile
 */
class XZERO_HTTP_API HttpMemoryFileRepository : public HttpFileRepository {
 public:
  explicit HttpMemoryFileRepository(MimeTypes& mimetypes);

  HttpFileRef getFile(const std::string& requestPath,
                      const std::string& docroot) override;

  void insert(const std::string& path, const char* data, size_t length);

 private:
  MimeTypes& mimetypes_;
  std::unordered_map<std::string, std::shared_ptr<HttpMemoryFile>> files_;
  std::shared_ptr<HttpMemoryFile> notFound_;
};

}  // namespace xzero

