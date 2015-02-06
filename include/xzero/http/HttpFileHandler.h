// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero/Api.h>
#include <string>
#include <functional>
#include <unordered_map>

namespace xzero {

class HttpRequest;
class HttpResponse;
class HttpFile;

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

  /**
   * Initializes static file handler.
   *
   * @param mtime whether or not to include Last-Modified timestamp
   * @param size whether or not to include file size
   * @param inode whether or not to include file's system inode
   * @param mimetypes if non-empty, given file will be loaded as mimetypes file.
   * @param defaultMimeType default mimetype to use if no mapping exists.
   * @param generateBoundaryID boundary-ID generator function that generates
   *                           response-local unique boundary IDs.
   */
  HttpFileHandler(bool mtime, bool size, bool inode,
                  const std::string& mimetypes,
                  const std::string& defaultMimeType,
                  std::function<std::string()> generateBoundaryID);

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
  const std::unordered_map<std::string, std::string>& mimetypes() const XZERO_NOEXCEPT;

  /** Retrieves the default mimetype. */
  const std::string& defaultMimeType() const XZERO_NOEXCEPT;

  /** Sets the default mimetype to given @p value. */
  void setDefaultMimeType(const std::string& value);

  /**
   * Handles given @p request if a local file (based on @p docroot) exists.
   *
   * Iff the given request was successfully handled, the response is
   * being also marked as completed, and thus, any future call to
   * the request or response object will be invalid.
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
  std::function<std::string()> generateBoundaryID_;

  // TODO stat cache
  // TODO fd cache
};

// {{{ inlines
inline const std::unordered_map<std::string, std::string>&
    HttpFileHandler::mimetypes() const XZERO_NOEXCEPT {
  return mimetypes_;
}

inline const std::string& HttpFileHandler::defaultMimeType() const XZERO_NOEXCEPT {
  return defaultMimeType_;
}

inline void HttpFileHandler::setDefaultMimeType(const std::string& value) {
  defaultMimeType_ = value;
}
// }}}

} // namespace xzero
