#include <xzero-base/io/FileUtil.h>
#include <xzero-base/RuntimeError.h>

#include <system_error>
#include <limits.h>
#include <stdlib.h>

namespace xzero {

std::string FileUtil::realpath(const std::string& relpath) {
  char result[PATH_MAX];
  if (::realpath(relpath.c_str(), result) == nullptr)
    throw std::system_error(errno, std::system_category());

  return result;
}

}  // namespace xzero
