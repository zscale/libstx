#include <cortex-http/fastcgi/Stream.h>
#include <cortex-http/fastcgi/Generator.h>
#include <cortex-base/StringUtil.h>
#include <cortex-base/logging.h>

#define TRACE(msg...) do { logTrace("fastcgi.Stream", msg); } while (0)

namespace cortex {
namespace http {
namespace fastcgi {

Stream::Stream(int rid, EndPointWriter* w, HttpChannel* ch)
    : requestId(rid),
      channel(ch),
      generator(rid, nullptr/*dateGenerator*/, w),
      onComplete(nullptr),
      environment(),
      requestContentActualLength(0),
      requestContentExpectedLength(0),
      paramsComplete(false),
      aborted(false) {
}

void Stream::onParam(
    const char *name, size_t nameLen,
    const char *value, size_t valueLen) {

  std::string nam(name, 0, nameLen);
  std::string val(value, 0, valueLen);

  if (nameLen > 5 && strncmp(name, "HTTP_", 5) == 0) {
    std::string key(nam.data(), 5, nam.size() - 5);
    StringUtil::replaceAll(&key, "_", "-");
    TRACE("onParam[header]: name(%s), value(%s)", key.c_str(), val.c_str());
    //listener->onMessageHeader(BufferRef(key), BufferRef(value));
  } else if (strcasecmp(nam.c_str(), "CONTENT_LENGTH") == 0) {
    TRACE("onParam[content-length]: name(%s), value(%s)", nam.c_str(), val.c_str());
    requestContentExpectedLength = atoi(val.c_str());
  } else {
    environment.push_back(std::make_pair(nam, val));
    TRACE("onParam[env]: name(%s), value(%s)", nam.c_str(), val.c_str());
  }
}

} // namespace fastcgi
} // namespace http
} // namespace cortex
