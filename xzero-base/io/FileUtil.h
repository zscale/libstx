#pragma once

#include <xzero-base/Api.h>
#include <functional>
#include <string>
#include <stdint.h>

namespace xzero {

class Buffer;
class File;

class XZERO_API FileDescriptor {
 public:
  FileDescriptor(int fd) : fd_(fd) {}

  FileDescriptor(FileDescriptor&& fd) : fd_(fd.fd_) {
    fd.fd_ = -1;
  }

  ~FileDescriptor() {
    close();
  }

  FileDescriptor& operator=(FileDescriptor&& fd) {
    close();

    fd_ = fd.fd_;
    fd.fd_ = -1;

    return *this;
  }

  void close();

  int get() const noexcept { return fd_; }
  operator int() const noexcept { return fd_; }

 private:
  int fd_;
};

class XZERO_API FileUtil {
 public:
  static char pathSeperator() noexcept;

  static std::string currentWorkingDirectory();

  static std::string realpath(const std::string& relpath);
  static bool exists(const std::string& path);
  static bool isDirectory(const std::string& path);
  static bool isRegular(const std::string& path);
  static size_t size(const std::string& path);
  static size_t sizeRecursive(const std::string& path);
  XZERO_DEPRECATED static size_t du_c(const std::string& path) { return sizeRecursive(path); }
  static void ls(const std::string& path, std::function<bool(const std::string&)> cb);

  static std::string joinPaths(const std::string& base, const std::string& append);

  static Buffer read(int fd);
  static Buffer read(File& file);
  static Buffer read(const std::string& path);
  static void write(const std::string& path, const Buffer& buffer);
  static void copy(const std::string& from, const std::string& to);
  static void truncate(const std::string& path, size_t size);
  static void mkdir(const std::string& path, int mode = 0775);
  static void mkdir_p(const std::string& path, int mode = 0775);
  static void rm(const std::string& path);
  static void mv(const std::string& path, const std::string& target);

  static int createTempFile();
  static std::string createTempDirectory();
  static std::string tempDirectory();
};

}  // namespace xzero
