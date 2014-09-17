#pragma once

#include <xzero/Api.h>
#include <xzero/http/HttpStatus.h>
#include <string>
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
  HttpFileHandler(bool mtime, bool size, bool inode);
  ~HttpFileHandler();

  void configureETag(bool mtime, bool size, bool inode);

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

  // TODO stat cache
  // TODO fd cache
};


} // namespace xzero
