#include <xzero-base/io/FileUtil.h>
#include <xzero-base/RuntimeError.h>
#include <xzero-base/Buffer.h>

#include <system_error>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>

namespace xzero {

std::string FileUtil::currentWorkingDirectory() {
  char buf[PATH_MAX];
  if (getcwd(buf, sizeof(buf)))
    return buf;

  throw RUNTIME_ERROR(strerror(errno));
}

std::string FileUtil::realpath(const std::string& relpath) {
  char result[PATH_MAX];
  if (::realpath(relpath.c_str(), result) == nullptr)
    throw RUNTIME_ERROR(strerror(errno));

  return result;
}

bool FileUtil::exists(const std::string& path) {
  struct stat st;
  return stat(path.c_str(), &st) == 0;
}

bool FileUtil::isDirectory(const std::string& path) {
  struct stat st;
  return stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

bool FileUtil::isRegular(const std::string& path) {
  struct stat st;
  return stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode);
}

size_t FileUtil::size(const std::string& path) {
  struct stat st;

  if (stat(path.c_str(), &st) == 0)
    return st.st_size;

  throw RUNTIME_ERROR(strerror(errno));
}

size_t FileUtil::sizeRecursive(const std::string& path) {
  struct stat st;
  if (stat(path.c_str(), &st) < 0)
    throw RUNTIME_ERROR(strerror(errno));

  if (S_ISREG(st.st_mode))
    return st.st_size;

  if (!S_ISDIR(st.st_mode))
    return 0;

  size_t totalSize = 0;
  FileUtil::ls(path, [&totalSize](const std::string& childpath) -> bool {
    totalSize += sizeRecursive(childpath);
    return true;
  });
  return totalSize;
}

void FileUtil::ls(const std::string& path,
                  std::function<bool(const std::string&)> callback) {
  DIR* dir = opendir(path.c_str());
  if (!dir)
    throw RUNTIME_ERROR(strerror(errno));

  int len = offsetof(dirent, d_name) + pathconf(path.c_str(), _PC_NAME_MAX);
  dirent* dep = (dirent*) new unsigned char[len + 1];
  dirent* res = nullptr;
  Buffer buf;

  std::string filename;
  filename = joinPaths(path, "/");
  std::size_t baseFileNameLength = filename.size();

  while (readdir_r(dir, dep, &res) == 0 && res) {
    // skip "."
    if (dep->d_name[0] == '.' && dep->d_name[1] == 0)
      continue;

    // skip ".."
    if (dep->d_name[0] == '.' && dep->d_name[1] == '.' && dep->d_name[2] == 0)
      continue;

    // prepare filename
    filename += static_cast<char*>(dep->d_name);

    // callback
    callback(filename);

    // reset filename to its base-length
    filename.resize(baseFileNameLength);
  }

  // free resources
  delete[] dep;
  closedir(dir);
}

std::string FileUtil::joinPaths(const std::string& base,
                                const std::string& append) {
  if (base.empty())
    return append;

  if (append.empty())
    return base;

  static const char PathSeperator = '/';

  if (base.back() == PathSeperator) {
    if (append.front() == PathSeperator) {
      return base + append.substr(1);
    } else {
      return base + append;
    }
  } else {
    if (append.front() == PathSeperator) {
      return base + append;
    } else {
      return base + '/' + append;
    }
  }
}

Buffer FileUtil::read(const std::string& path) {
  Buffer output;

  int fd = open(path.c_str(), O_RDONLY);
  if (fd < 0)
    throw RUNTIME_ERROR(strerror(errno));

  struct stat st;
  if (fstat(fd, &st) < 0)
    throw RUNTIME_ERROR(strerror(errno));

  output.reserve(st.st_size + 1);
  ssize_t nread = ::read(fd, output.data(), st.st_size);
  if (nread < 0) {
    ::close(fd);
    throw RUNTIME_ERROR(strerror(errno));
  }

  output.data()[nread] = '\0';
  output.resize(nread);
  ::close(fd);

  return output;
}

void FileUtil::write(const std::string& path, const Buffer& buffer) {
  int fd = open(path.c_str(), O_WRONLY | O_CREAT);
  if (fd < 0)
    throw RUNTIME_ERROR(strerror(errno));

  ssize_t nwritten = 0;
  do {
    ssize_t rv = ::write(fd, buffer.data(), buffer.size());
    if (rv < 0) {
      ::close(fd);
      throw RUNTIME_ERROR(strerror(errno));
    }
    nwritten += rv;
  } while (static_cast<size_t>(nwritten) < buffer.size());

  ::close(fd);
}

void FileUtil::copy(const std::string& from, const std::string& to) {
  throw RUNTIME_ERROR("TODO");
}

void FileUtil::truncate(const std::string& path, size_t size) {
  if (::truncate(path.c_str(), size) < 0)
    throw RUNTIME_ERROR(strerror(errno));
}

void FileUtil::mkdir(const std::string& path, int mode) {
  if (::mkdir(path.c_str(), mode) < 0)
    throw RUNTIME_ERROR(strerror(errno));
}

void FileUtil::mkdir_p(const std::string& path, int mode) {
  throw RUNTIME_ERROR("TODO");
}

void FileUtil::rm(const std::string& path) {
  if (::unlink(path.c_str()) < 0)
    throw RUNTIME_ERROR(strerror(errno));
}

void FileUtil::mv(const std::string& path, const std::string& target) {
  if (::rename(path.c_str(), target.c_str()) < 0)
    throw RUNTIME_ERROR(strerror(errno));
}

int FileUtil::createTempFile() {
  throw RUNTIME_ERROR("TODO");
}

std::string FileUtil::createTempDir() {
  throw RUNTIME_ERROR("TODO");
}

}  // namespace xzero
