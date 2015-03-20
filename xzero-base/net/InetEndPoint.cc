// This file is part of the "libxzero" project
//   (c) 2009-2015 Christian Parpart <https://github.com/christianparpart>
//   (c) 2014-2015 Paul Asmuth <https://github.com/paulasmuth>
//
// libxzero is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License v3.0. You should have received a
// copy of the GNU General Public License along with this program. If not, see
// <http://www.gnu.org/licenses/>.

#include <xzero-base/net/InetEndPoint.h>
#include <xzero-base/net/InetConnector.h>
#include <xzero-base/net/Connection.h>
#include <xzero-base/executor/Executor.h>
#include <xzero-base/logging.h>
#include <xzero-base/RuntimeError.h>
#include <xzero-base/Buffer.h>
#include <xzero-base/sysconfig.h>
#include <xzero-base/RefPtr.h>
#include <stdexcept>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#if defined(HAVE_SYS_SENDFILE_H)
#include <sys/sendfile.h>
#endif

namespace xzero {

#define ERROR(msg...) logError("net.InetEndPoint", msg)

#ifndef NDEBUG
#define TRACE(msg...) logTrace("net.InetEndPoint", msg)
#else
#define TRACE(msg...) do {} while (0)
#endif

InetEndPoint::InetEndPoint(int socket,
                           InetConnector* connector,
                           Scheduler* scheduler)
    : EndPoint(),
      connector_(connector),
      scheduler_(scheduler),
      idleTimeout_(connector->clock(), connector->scheduler()),
      io_(),
      handle_(socket),
      isCorking_(false) {

  idleTimeout_.setCallback(std::bind(&InetEndPoint::onTimeout, this));
  idleTimeout_.setTimeout(connector->idleTimeout());
}

void InetEndPoint::onTimeout() {
  if (connection()) {
    if (connection()->onReadTimeout()) {
      close();
    }
  }
}

InetEndPoint::~InetEndPoint() {
  if (isOpen()) {
    close();
  }
}

std::pair<IPAddress, int> InetEndPoint::remoteAddress() const {
  if (handle_ < 0)
    RAISE(IllegalStateError);

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
      RAISE(IllegalStateError);
  }
  return result;
}

std::pair<IPAddress, int> InetEndPoint::localAddress() const {
  if (handle_ < 0)
    RAISE(IllegalStateError);

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

bool InetEndPoint::isOpen() const XZERO_NOEXCEPT {
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
  TRACE("%p setBlocking(\"%s\")", this, enable ? "blocking" : "nonblocking");
#if 1
  unsigned current = fcntl(handle_, F_GETFL);
  unsigned flags = enable ? (current & ~O_NONBLOCK)
                          : (current | O_NONBLOCK);
#else
  unsigned flags = enable ? fcntl(handle_, F_GETFL) & ~O_NONBLOCK
                          : fcntl(handle_, F_GETFL) | O_NONBLOCK;
#endif

  if (fcntl(handle_, F_SETFL, flags) < 0) {
    RAISE_ERRNO(errno);
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
      RAISE_ERRNO(errno);

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

  if (n < 0) {
    // don't raise on soft errors, such as there is simply no more data to read.
    switch (errno) {
      case EBUSY:
      case EAGAIN:
#if defined(EWOULDBLOCK) && (EWOULDBLOCK != EAGAIN)
      case EWOULDBLOCK:
#endif
        break;
      default:
        RAISE_ERRNO(errno);
    }
  } else {
    result->resize(result->size() + n);
  }

  return n;
}

size_t InetEndPoint::flush(const BufferRef& source) {
  ssize_t rv = write(handle(), source.data(), source.size());

  if (rv < 0)
    RAISE_ERRNO(errno);

  // EOF exception?

  return rv;
}

size_t InetEndPoint::flush(int fd, off_t offset, size_t size) {
#if defined(__APPLE__)
  off_t len = 0;
  int rv = sendfile(fd, handle(), offset, &len, nullptr, 0);
  if (rv < 0)
    RAISE_ERRNO(errno);

  return len;
#else
  ssize_t rv = sendfile(handle(), fd, &offset, size);
  if (rv < 0)
    RAISE_ERRNO(errno);

  // EOF exception?

  return rv;
#endif
}

void InetEndPoint::onReadable() XZERO_NOEXCEPT {
  RefPtr<EndPoint> _guard(this);

  try {
    connection()->onFillable();
  } catch (const std::exception& e) {
    connection()->onInterestFailure(e);
  } catch (...) {
    connection()->onInterestFailure(
        EXCEPTION(RuntimeError, (int) Status::CaughtUnknownExceptionError,
                  StatusCategory::get()));
  }
}

void InetEndPoint::onWritable() XZERO_NOEXCEPT {
  RefPtr<EndPoint> _guard(this);

  try {
    connection()->onFlushable();
  } catch (const std::exception& e) {
    connection()->onInterestFailure(e);
  } catch (...) {
    connection()->onInterestFailure(
        EXCEPTION(RuntimeError, (int) Status::CaughtUnknownExceptionError,
                  StatusCategory::get()));
  }
}

void InetEndPoint::wantFill() {
  TRACE("%p wantFill()", this);
  // TODO: abstract away the logic of TCP_DEFER_ACCEPT

  //idleTimeout_.activate();
  if (!io_) {
    io_ = scheduler_->executeOnReadable(
        handle(),
        std::bind(&InetEndPoint::fillable, this));
  }
}

void InetEndPoint::fillable() {
  RefPtr<EndPoint> _guard(this);

  try {
    io_.reset();
    connection()->onFillable();
  } catch (const std::exception& e) {
    connection()->onInterestFailure(e);
  } catch (...) {
    connection()->onInterestFailure(
        EXCEPTION(RuntimeError, (int) Status::CaughtUnknownExceptionError,
                  StatusCategory::get()));
  }
}

void InetEndPoint::wantFlush() {
  TRACE("%p wantFlush() %s", this, io_.get() ? "again" : "first time");
  //idleTimeout_.activate();

  if (!io_) {
    io_ = scheduler_->executeOnWritable(
        handle(),
        std::bind(&InetEndPoint::flushable, this));
  }
}

void InetEndPoint::flushable() {
  RefPtr<EndPoint> _guard(this);

  try {
    io_.reset();
    connection()->onFlushable();
  } catch (const std::exception& e) {
    connection()->onInterestFailure(e);
  } catch (...) {
    connection()->onInterestFailure(
        EXCEPTION(RuntimeError, (int) Status::CaughtUnknownExceptionError,
                  StatusCategory::get()));
  }
}

TimeSpan InetEndPoint::idleTimeout() {
  return idleTimeout_.timeout();
}

void InetEndPoint::setIdleTimeout(TimeSpan timeout) {
  idleTimeout_.setTimeout(timeout);
}

} // namespace xzero
