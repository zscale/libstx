#include <xzero-http/HttpLocalFileRepository.h>
#include <xzero-http/HttpLocalFile.h>
#include <xzero-base/MimeTypes.h>

namespace xzero {

HttpLocalFileRepository::HttpLocalFileRepository(MimeTypes& mt,
                                                 bool etagMtime,
                                                 bool etagSize,
                                                 bool etagInode)
    : mimetypes_(mt),
      etagConsiderMTime_(etagMtime),
      etagConsiderSize_(etagSize),
      etagConsiderINode_(etagInode) {
}

HttpFileRef HttpLocalFileRepository::getFile(const std::string& requestPath,
                      const std::string& docroot) {
  std::string path = docroot + requestPath;

  return HttpFileRef(new HttpLocalFile(
        *this, path, mimetypes_.getMimeType(requestPath)));
}

void HttpLocalFileRepository::configureETag(bool mtime, bool size, bool inode) {
  etagConsiderMTime_ = mtime;
  etagConsiderSize_ = size;
  etagConsiderINode_ = inode;
}

}  // namespace xzero
