#pragma once

#include <xzero-base/Api.h>
#include <functional>
#include <string>
#include <stdint.h>

namespace xzero {

class Buffer;

class XZERO_API FileUtil {
 public:
  static std::string currentWorkingDirectory();

  static std::string realpath(const std::string& relpath);
  static bool exists(const std::string& path);
  static bool isDirectory(const std::string& path);
  static bool isRegular(const std::string& path);
  static size_t size(const std::string& path);
  static size_t du_c(const std::string& path);
  static void ls(const std::string& path, std::function<bool(const std::string&)> cb);

  static std::string joinPaths(const std::string& base, const std::string& append);

  static void mkdir(const std::string& path);
  static void mkdir_p(const std::string& path);
  static void rm(const std::string& path);
  static void mv(const std::string& path, const std::string& target);
  static void truncate(const std::string& path, size_t size);
  static Buffer read(const std::string& path);
  static void write(const std::string& path, const Buffer& buffer);
  static void copy(const std::string& from, const std::string& to);
};

}  // namespace xzero
