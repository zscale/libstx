// This file is part of the "x0" project
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <xzero-base/Api.h>
#include <xzero-base/io/FileRepository.h>
#include <xzero-base/io/MemoryFile.h>
#include <unordered_map>
#include <string>

namespace xzero {

class MimeTypes;

/**
 * In-memory file store.
 *
 * @see LocalFileRepository
 * @see FileHandler
 * @see MemoryFile
 */
class XZERO_API MemoryFileRepository : public FileRepository {
 public:
  explicit MemoryFileRepository(MimeTypes& mimetypes);

  std::shared_ptr<File> getFile(
      const std::string& requestPath,
      const std::string& docroot) override;

  void listFiles(std::function<bool(const std::string&)> callback) override;
  void deleteAllFiles() override;

  void insert(const std::string& path, const BufferRef& data, DateTime mtime);

  void insert(const std::string& path, const BufferRef& data);

 private:
  MimeTypes& mimetypes_;
  std::unordered_map<std::string, std::shared_ptr<MemoryFile>> files_;
  std::shared_ptr<MemoryFile> notFound_;
};

}  // namespace xzero

