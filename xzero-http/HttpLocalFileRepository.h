// This file is part of the "x0" project
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <xzero-http/Api.h>
#include <xzero-http/HttpFileRepository.h>

namespace xzero {

class MimeTypes;
class HttpLocalFile;

class XZERO_HTTP_API HttpLocalFileRepository : public HttpFileRepository {
 public:
  /**
   * Initializes local file repository.
   *
   * @param mimetypes mimetypes database to use for creating entity tags.
   * @param basedir base directory to start all lookups from (like "/").
   * @param etagMtime whether or not to include Last-Modified timestamp in etag.
   * @param etagSize whether or not to include file size in etag.
   * @param etagInode whether or not to include file's system inode in etag.
   */
  HttpLocalFileRepository(
      MimeTypes& mimetypes,
      const std::string& basedir,
      bool etagMtime, bool etagSize, bool etagInode);

  const std::string baseDirectory() const { return basedir_; }

  HttpFileRef getFile(const std::string& requestPath,
                      const std::string& docroot) override;

  /**
   * Configures ETag generation.
   *
   * @param mtime whether or not to include Last-Modified timestamp
   * @param size whether or not to include file size
   * @param inode whether or not to include file's system inode
   */
  void configureETag(bool mtime, bool size, bool inode);

 private:
  friend class HttpLocalFile;

  MimeTypes& mimetypes_;
  std::string basedir_;
  bool etagConsiderMTime_;
  bool etagConsiderSize_;
  bool etagConsiderINode_;
};

}  // namespace xzero

