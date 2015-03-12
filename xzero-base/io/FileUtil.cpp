#include <xzero-base/io/FileUtil.h>
#include <xzero-base/io/File.h>
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

FileDescriptor& FileDescriptor::operator=(FileDescriptor&& fd) {
  close();
  fd_ = fd.release();

  return *this;
}

int FileDescriptor::release() {
  int fd = fd_;
  fd_ = -1;
  return fd;
}

void FileDescriptor::close() {
  if (fd_ >= 0) {
    ::close(fd_);
    fd_ = -1;
  }
}

static const char PathSeperator = '/';

char FileUtil::pathSeperator() noexcept {
  return PathSeperator;
}

std::string FileUtil::currentWorkingDirectory() {
  char buf[PATH_MAX];
  if (getcwd(buf, sizeof(buf)))
    return buf;

  RAISE_ERRNO(errno);
}

std::string FileUtil::realpath(const std::string& relpath) {
  char result[PATH_MAX];
  if (::realpath(relpath.c_str(), result) == nullptr)
    RAISE_ERRNO(errno);

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

  RAISE_ERRNO(errno);
}

size_t FileUtil::sizeRecursive(const std::string& path) {
  struct stat st;
  if (stat(path.c_str(), &st) < 0)
    RAISE_ERRNO(errno);

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
    RAISE_ERRNO(errno);

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

Buffer FileUtil::read(int fd) {
  Buffer output;

  struct stat st;
  if (fstat(fd, &st) < 0)
    RAISE_ERRNO(errno);

  output.reserve(st.st_size + 1);
  ssize_t nread = ::pread(fd, output.data(), st.st_size, 0);
  if (nread < 0) {
    ::close(fd);
    RAISE_ERRNO(errno);
  }

  output.data()[nread] = '\0';
  output.resize(nread);

  return output;
}

Buffer FileUtil::read(File& file) {
  FileDescriptor fd = file.tryCreateChannel();
  return read(fd);
}

Buffer FileUtil::read(const std::string& path) {
  FileDescriptor fd = open(path.c_str(), O_RDONLY);
  if (fd < 0)
    RAISE_ERRNO(errno);

  return read(fd);
}

void FileUtil::write(const std::string& path, const Buffer& buffer) {
  int fd = open(path.c_str(), O_WRONLY | O_CREAT);
  if (fd < 0)
    RAISE_ERRNO(errno);

  ssize_t nwritten = 0;
  do {
    ssize_t rv = ::write(fd, buffer.data(), buffer.size());
    if (rv < 0) {
      ::close(fd);
      RAISE_ERRNO(errno);
    }
    nwritten += rv;
  } while (static_cast<size_t>(nwritten) < buffer.size());

  ::close(fd);
}

void FileUtil::copy(const std::string& from, const std::string& to) {
  RAISE(RuntimeError, "TODO");
}

void FileUtil::truncate(const std::string& path, size_t size) {
  if (::truncate(path.c_str(), size) < 0)
    RAISE_ERRNO(errno);
}

void FileUtil::mkdir(const std::string& path, int mode) {
  if (::mkdir(path.c_str(), mode) < 0)
    RAISE_ERRNO(errno);
}

void FileUtil::mkdir_p(const std::string& path, int mode) {
  RAISE(RuntimeError, "TODO");
}

void FileUtil::rm(const std::string& path) {
  if (::unlink(path.c_str()) < 0)
    RAISE_ERRNO(errno);
}

void FileUtil::mv(const std::string& path, const std::string& target) {
  if (::rename(path.c_str(), target.c_str()) < 0)
    RAISE_ERRNO(errno);
}

int FileUtil::createTempFile() {
  RAISE(RuntimeError, "TODO");
}

std::string FileUtil::createTempDirectory() {
  std::string path;
  path += tempDirectory();
  path += PathSeperator;
  path += "xzero.XXXXXXXX";

  if (!mkdtemp(const_cast<char*>(path.c_str())))
    RAISE_ERRNO(errno);

  return path;
}

std::string FileUtil::tempDirectory() {
  if (const char* s = getenv("TMPDIR"))
    return s;

  if (const char* s = getenv("TEMPDIR"))
    return s;

  if (const char* s = getenv("TEMP"))
    return s;

  return "/tmp";
}

}  // namespace xzero
