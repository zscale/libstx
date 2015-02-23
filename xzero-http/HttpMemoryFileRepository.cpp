#include <xzero-http/HttpMemoryFileRepository.h>
#include <xzero-base/MimeTypes.h>

namespace xzero {

HttpMemoryFileRepository::HttpMemoryFileRepository(MimeTypes& mimetypes)
    : mimetypes_(mimetypes),
      files_(),
      notFound_(new HttpMemoryFile()) {
}

HttpFileRef HttpMemoryFileRepository::getFile(
    const std::string& requestPath,
    const std::string& /*docroot*/) {
  auto i = files_.find(requestPath);
  if (i != files_.end())
    return i->second;

  return notFound_;
}

void HttpMemoryFileRepository::insert(
    const std::string& path, const char* data, size_t length) {
  files_[path].reset(new HttpMemoryFile(path, data, length,
        mimetypes_.getMimeType(path)));
}

} // namespace xzero
