#include <xzero-base/io/MemoryFileRepository.h>
#include <xzero-base/RuntimeError.h>
#include <xzero-base/MimeTypes.h>

namespace xzero {

MemoryFileRepository::MemoryFileRepository(MimeTypes& mimetypes)
    : mimetypes_(mimetypes),
      files_(),
      notFound_(new MemoryFile()) {
}

std::shared_ptr<File> MemoryFileRepository::getFile(
    const std::string& requestPath,
    const std::string& /*docroot*/) {
  auto i = files_.find(requestPath);
  if (i != files_.end())
    return i->second;

  return notFound_;
}

void MemoryFileRepository::listFiles(
    std::function<bool(const std::string&)> callback) {
  for (auto& file: files_) {
    if (!callback(file.first)) {
      break;
    }
  }
}

void MemoryFileRepository::deleteAllFiles() {
  files_.clear();
}

int MemoryFileRepository::createTempFile(std::string* filename) {
  RAISE(RuntimeError, "TODO");
}

void MemoryFileRepository::insert(
    const std::string& path, const BufferRef& data, DateTime mtime) {
  files_[path].reset(new MemoryFile(path,
                                    mimetypes_.getMimeType(path),
                                    data,
                                    mtime));
}

void MemoryFileRepository::insert(
    const std::string& path, const BufferRef& data) {
  insert(path, data, DateTime());
}

} // namespace xzero
