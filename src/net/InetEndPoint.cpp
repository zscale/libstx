#include <xzero/net/InetEndPoint.h>
#include <xzero/net/InetConnector.h>
#include <xzero/net/Connection.h>
#include <xzero/executor/Executor.h>
#include <xzero/io/SelectionKey.h>
#include <xzero/logging/LogSource.h>
#include <xzero/Buffer.h>
#include <xzero/sysconfig.h>
#include <system_error>
#include <stdexcept>
#include <netinet/tcp.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <fcntl.h>

namespace xzero {

static LogSource inetEndPointLogger("net.InetEndPoint");
#ifndef NDEBUG
#define TRACE(msg...) do { inetEndPointLogger.trace(msg); } while (0)
#define ERROR(msg...) do { inetEndPointLogger.error(msg); } while (0)
#else
#define TRACE(msg...) do {} while (0)
#define ERROR(msg...) do {} while (0)
#endif

InetEndPoint::InetEndPoint(int socket, InetConnector* connector)
    : EndPoint(),
      connector_(connector),
      idleTimeout_(connector->clock(), connector->scheduler()),
      handle_(socket),
      isCorking_(false),
      isBusy_(0) {
}

InetEndPoint::~InetEndPoint() {
  if (isOpen()) {
    close();
  }
}

std::pair<IPAddress, int> InetEndPoint::remoteAddress() const {
  if (handle_ < 0)
    throw std::runtime_error("Illegal State. Socket is closed.");

  std::pair<IPAddress, int> result;
  switch (connector_->addressFamily()) {
    case AF_INET6: {
      sockaddr_in6 saddr;
      socklen_t slen = sizeof(saddr);
      if (getpeername(handle_, (sockaddr*)&saddr, &slen) == 0) {
        result.first = IPAddress(&saddr);
        result.second = ntohs(saddr.sin6_port);
      }
      break;
    }
    case AF_INET: {
      sockaddr_in saddr;
      socklen_t slen = sizeof(saddr);
      if (getpeername(handle_, (sockaddr*)&saddr, &slen) == 0) {
        result.first = IPAddress(&saddr);
        result.second = ntohs(saddr.sin_port);
      }
      break;
    }
    default:
      throw std::runtime_error("Illegal State. IPAddress.addressFamily?");
  }
  return result;
}

std::pair<IPAddress, int> InetEndPoint::localAddress() const {
  if (handle_ < 0)
    throw std::runtime_error("Illegal State. Socket is closed.");

  std::pair<IPAddress, int> result;
  switch (connector_->addressFamily()) {
    case AF_INET6: {
      sockaddr_in6 saddr;
      socklen_t slen = sizeof(saddr);

      if (getsockname(handle_, (sockaddr*)&saddr, &slen) == 0) {
        result.first = IPAddress(&saddr);
        result.second = ntohs(saddr.sin6_port);
      }
      break;
    }
    case AF_INET: {
      sockaddr_in saddr;
      socklen_t slen = sizeof(saddr);

      if (getsockname(handle_, (sockaddr*)&saddr, &slen) == 0) {
        result.first = IPAddress(&saddr);
        result.second = ntohs(saddr.sin_port);
      }
      break;
    }
    default:
      break;
  }

  return result;
}

int InetEndPoint::handle() const noexcept {
  return handle_;
}

Selector* InetEndPoint::selector() const noexcept {
  return connector_->selector();
}

void InetEndPoint::setHandle(int handle) noexcept {
  handle_ = handle;
}

bool InetEndPoint::isOpen() const noexcept {
  return handle_ >= 0;
}

void InetEndPoint::close() {
  if (isOpen()) {
    ::close(handle_);
    handle_ = -1;

    if (connector_) {
      connector_->onEndPointClosed(this);
    }
  }
}

bool InetEndPoint::isBlocking() const {
  return !(fcntl(handle_, F_GETFL) & O_NONBLOCK);
}

void InetEndPoint::setBlocking(bool enable) {
  unsigned flags = enable ? fcntl(handle_, F_GETFL) & ~O_NONBLOCK
                          : fcntl(handle_, F_GETFL) | O_NONBLOCK;

  if (fcntl(handle_, F_SETFL, flags) < 0) {
    throw std::system_error(errno, std::system_category());
  }
}

bool InetEndPoint::isCorking() const {
  return isCorking_;
}

void InetEndPoint::setCorking(bool enable) {
#if defined(TCP_CORK)
  if (isCorking_ != enable) {
    int flag = enable ? 1 : 0;
    if (setsockopt(handle_, IPPROTO_TCP, TCP_CORK, &flag, sizeof(flag)) < 0)
      throw std::system_error(errno, std::system_category());

    isCorking_ = enable;
  }
#endif
}

std::string InetEndPoint::toString() const {
  char buf[32];
  snprintf(buf, sizeof(buf), "InetEndPoint(%d)@%p", handle(), this);
  return buf;
}

size_t InetEndPoint::fill(Buffer* result) {
  result->reserve(result->size() + 1024);
  ssize_t n = read(handle(), result->end(), result->capacity() - result->size());

  if (n < 0)
    throw std::system_error(errno, std::system_category());

  result->resize(result->size() + n);
  return n;
}

size_t InetEndPoint::flush(const BufferRef& source) {
  ssize_t rv = write(handle(), source.data(), source.size());

  if (rv < 0)
    throw std::system_error(errno, std::system_category());

  // EOF exception?

  return rv;
}

size_t InetEndPoint::flush(int fd, off_t offset, size_t size) {
  ssize_t rv = sendfile(handle(), fd, &offset, size);

  if (rv < 0)
    throw std::system_error(errno, std::system_category());

  // EOF exception?

  return rv;
}

void InetEndPoint::wantFill() {
  // TODO: abstract away the logic of TCP_DEFER_ACCEPT

  if (selector()) {
    selectionKey_ = registerSelectable(READ);
  } else {
    isBusy_++;
    try {
      connection()->onFillable();
      isBusy_--;
    } catch (const std::exception& e) {
      TRACE("wantFill: exception caught: %s", e.what()); connection()->onInterestFailure(e);
      isBusy_--;
    }
    if (!isBusy_ && isClosed()) {
      TRACE("wantFill: isClosed == true, releasing");
      connector_->release(connection());
    }
  }
}

void InetEndPoint::onSelectable() noexcept {
  TRACE("InetEndPoint.onSelectable()%s%s\n",
        selectionKey_->isReadable() ? " read" : "",
        selectionKey_->isWriteable() ? " write" : "");

  isBusy_++;

  try {
    if (selectionKey_->isReadable()) {
      connection()->onFillable();
    }

    if (selectionKey_->isWriteable()) {
      connection()->onFlushable();
    }

    isBusy_--;
  } catch (const std::exception& e) {
    ERROR("%p onSelectable: unhandled exception caught. %s %s", this,
          typeid(e).name(), e.what());
    connection()->onInterestFailure(e);
    isBusy_--;
  }

  if (!isBusy_ && isClosed()) {
    connector_->release(connection());
  }
}

void InetEndPoint::wantFlush(bool enable) {
  if (selector()) {
    if (selectionKey_) {
      int cur = selectionKey_->interest();
      int newInterests = enable ? (cur | WRITE) : (cur & ~WRITE);
      selectionKey_->change(newInterests);
    } else {
      selectionKey_ = registerSelectable(WRITE);
    }
  } else if (enable) {
    isBusy_++;
    try {
      connection()->onFlushable();
      isBusy_--;
    } catch (const std::exception& e) {
      TRACE("wantFlush: exception caught: %s", e.what()); connection()->onInterestFailure(e);
      isBusy_--;
    }
    if (!isBusy_ && isClosed()) {
      TRACE("wantFlush: isClosed == true, releasing");
      connector_->release(connection());
    }
  }
}

TimeSpan InetEndPoint::idleTimeout() {
  return idleTimeout_.timeout();
}

void InetEndPoint::setIdleTimeout(TimeSpan timeout) {
  idleTimeout_.setTimeout(timeout);
}

} // namespace xzero
