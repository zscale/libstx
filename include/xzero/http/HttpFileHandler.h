// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero/Api.h>
#include <xzero/http/HttpStatus.h>
#include <string>
#include <unordered_map>
#include <sys/stat.h>

namespace xzero {

class HttpRequest;
class HttpResponse;

/**
 * HTTP servable file.
 *
 * @see HttpFileHandler
 */
class XZERO_API HttpFile {
 public:
  HttpFile(const HttpFile&) = delete;
  HttpFile& operator=(const HttpFile&) = delete;

  HttpFile(const std::string& path, const std::string& mimetype, bool mtime,
           bool size, bool inode);

  ~HttpFile();

  bool isError() const noexcept { return errno_ != 0; }
  int errorCode() const noexcept { return errno_; }

  const std::string& path() const { return path_; }
  std::string filename() const;
  const std::string& mimetype() const { return mimetype_; }
  const std::string& etag() const;
  const std::string& lastModified() const;

  size_t size() const noexcept { return stat_.st_size; }
  time_t mtime() const noexcept { return stat_.st_mtime; }
  operator const struct stat*() const { return &stat_; }
  const struct stat* operator->() const { return &stat_; }

  bool isRegular() const noexcept { return S_ISREG(stat_.st_mode); }

  void update();

  /** Creates a file descriptor for this file. */
  int tryCreateChannel();

 private:
  std::string path_;
  int errno_;
  std::string mimetype_;
  bool etagConsiderMTime_;
  bool etagConsiderSize_;
  bool etagConsiderINode_;
  mutable std::string etag_;
  mutable std::string lastModified_;
  struct stat stat_;
};

/**
 * Handles GET/HEAD requests to local files.
 *
 * @note this class is not meant to be thread safe.
 */
class XZERO_API HttpFileHandler {
 public:
  /**
   * Initializes static file handler.
   *
   * @param mtime whether or not to include Last-Modified timestamp
   * @param size whether or not to include file size
   * @param inode whether or not to include file's system inode
   */
  HttpFileHandler(bool mtime, bool size, bool inode);

  /**
   * Initializes static file handler.
   *
   * @param mtime whether or not to include Last-Modified timestamp
   * @param size whether or not to include file size
   * @param inode whether or not to include file's system inode
   * @param mimetypes if non-empty, given file will be loaded as mimetypes file.
   * @param defaultMimeType default mimetype to use if no mapping exists.
   */
  HttpFileHandler(bool mtime, bool size, bool inode,
                  const std::string& mimetypes,
                  const std::string& defaultMimeType);

  ~HttpFileHandler();

  /**
   * Configures ETag generation.
   *
   * @param mtime whether or not to include Last-Modified timestamp
   * @param size whether or not to include file size
   * @param inode whether or not to include file's system inode
   */
  void configureETag(bool mtime, bool size, bool inode);

  /** Loads the mimetype map from given local file at @p path. */
  void loadMimeTypesFromLocal(const std::string& path);

  /** Retrieves a mimetype based on given file @p path name. */
  std::string getMimeType(const std::string& path);

  /** Retrieves the mimetype mappings (from file extension to mimetype). */
  const std::unordered_map<std::string, std::string>& mimetypes() const noexcept;

  /** Retrieves the default mimetype. */
  const std::string& defaultMimeType() const noexcept;

  /** Sets the default mimetype to given @p value. */
  void setDefaultMimeType(const std::string& value);

  /**
   * Handles given @p request if a local file (based on @p docroot) exists.
   *
   * @param request the request to handle.
   * @param response the response to generate.
   * @param docroot request's document root into the local file system.
   */
  bool handle(HttpRequest* request, HttpResponse* response,
              const std::string& docroot);

 private:
  /**
   * Evaluates conditional requests to local file.
   *
   * @param transferFile the targeted file that is meant to be transferred.
   * @param fd open file descriptor in case of a GET request.
   * @param request HTTP request handle.
   * @param response HTTP response handle.
   *
   * @retval true request was fully handled, e.g.
   *              HttpResponse::completed() was invoked.
   * @retval false Could not handle request.
   */
  bool handleClientCache(const HttpFile& transferFile, HttpRequest* request,
                         HttpResponse* response);

  /**
   * Fully processes the ranged requests, if one, or does nothing.
   *
   * @param transferFile
   * @param fd open file descriptor in case of a GET request.
   * @param request HTTP request handle.
   * @param response HTTP response handle.
   *
   * @retval true this was a ranged request and we fully processed it, e.g.
   *              HttpResponse:::completed() was invoked.
   * @retval false this is no ranged request.
   *
   * @note if this is no ranged request. nothing is done on it.
   */
  bool handleRangeRequest(const HttpFile& transferFile, int fd,
                          HttpRequest* request, HttpResponse* response);

 private:
  bool etagConsiderMTime_;
  bool etagConsiderSize_;
  bool etagConsiderINode_;
  std::unordered_map<std::string, std::string> mimetypes_;
  std::string defaultMimeType_;

  // TODO stat cache
  // TODO fd cache
};

// {{{ inlines
inline const std::unordered_map<std::string, std::string>&
    HttpFileHandler::mimetypes() const noexcept {
  return mimetypes_;
}

inline const std::string& HttpFileHandler::defaultMimeType() const noexcept {
  return defaultMimeType_;
}

inline void HttpFileHandler::setDefaultMimeType(const std::string& value) {
  defaultMimeType_ = value;
}
// }}}

} // namespace xzero
