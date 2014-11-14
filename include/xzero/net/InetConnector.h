// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero/Api.h>
#include <xzero/sysconfig.h>
#include <xzero/io/Selectable.h>
#include <xzero/net/Connector.h>
#include <xzero/net/IPAddress.h>
#include <xzero/TimeSpan.h>
#include <list>
#include <deque>
#include <mutex>

namespace xzero {

class Connection;
class Executor;
class Scheduler;
class Selector;
class InetEndPoint;

/**
 * TCP/IP Internet Connector API
 */
class XZERO_API InetConnector : public Connector, public Selectable {
 public:
  /**
   * Initializes this connector.
   *
   * @param name Describing name for this connector.
   * @param executor Executor service to run handlers on
   * @param scheduler Scheduler service to use for timeout management
   * @param selector Selector service to use for I/O multiplexing
   * @param clock Wall clock used for timeout management.
   * @param idleTimeout timespan indicating how long a connection may be idle.
   * @param ipaddress TCP/IP address to listen on
   * @param port TCP/IP port number to listen on
   * @param backlog TCP backlog for this listener.
   * @param reuseAddr Flag indicating @c SO_REUSEADDR.
   * @param reusePort Flag indicating @c SO_REUSEPORT.
   *
   * @throw std::runtime_error on any kind of runtime error.
   */
  InetConnector(const std::string& name, Executor* executor,
                Scheduler* scheduler, Selector* selector, WallClock* clock,
                TimeSpan idleTimeout,
                const IPAddress& ipaddress, int port, int backlog,
                bool reuseAddr, bool reusePort);

  /**
   * Minimal initializer.
   *
   * @param name Describing name for this connector.
   * @param executor Executor service to run on
   * @param scheduler Scheduler service to use for timeout management
   * @param selector Selector service to use for I/O multiplexing
   * @param clock Wall clock used for timeout management.
   * @param idleTimeout timespan indicating how long a connection may be idle.
   */
  InetConnector(const std::string& name, Executor* executor,
                Scheduler* scheduler, Selector* selector, WallClock* clock,
                TimeSpan idleTimeout);

  ~InetConnector();

  Scheduler* scheduler() const XZERO_NOEXCEPT;

  Selector* selector() const XZERO_NOEXCEPT override;

  /**
   * Opens this connector by binding to the given @p ipaddress and @p port.
   *
   * @param ipaddress TCP/IP address to listen on
   * @param port TCP/IP port number to listen on
   * @param backlog TCP backlog for this listener.
   * @param reuseAddr Flag indicating @c SO_REUSEADDR.
   * @param reusePort Flag indicating @c SO_REUSEPORT.
   *
   * @throw std::runtime_error on any kind of runtime error.
   */
  void open(const IPAddress& ipaddress, int port, int backlog, bool reuseAddr,
            bool reusePort);

  /**
   * Tests whether this connector is open.
   */
  bool isOpen() const XZERO_NOEXCEPT;

  /**
   * Implicitely stops and finally closes this connnector.
   */
  void close();

  /**
   * Retrieves the underlying system socket handle.
   */
  virtual int handle() const XZERO_NOEXCEPT;

  /**
   * Returns the IP address family, such as @c IPAddress::V4 or @c IPAddress::V6.
   */
  int addressFamily() const { return addressFamily_; }

  /**
   * Sets the underlying system socket handle.
   */
  void setSocket(int socket);

  size_t backlog() const XZERO_NOEXCEPT;
  void setBacklog(size_t enable);

  /** Tests wether this connector is blocking on accepting new clients. */
  bool isBlocking() const;

  /**
   * Specifies whether accepting new clients should block or not.
   *
   * @param enable true, if accepting a new client should block, false otherwise
   *
   * This flag is inherited by newly created client endpoints as effecient
   * as possible.
   *
   * That is, a non-blocking connector will create non-blocking endpoints
   * for the newly accepted clients.
   *
   * @see EndPoint::setBlocking(bool enable)
   */
  void setBlocking(bool enable);

  /**
   * Tests whether the underlying system handle is closed on exec() syscalls.
   */
  bool closeOnExec() const;

  /**
   * Enables/disables the auto-close flag on exec()-family system calls.
   *
   * This flag is inherited by newly created client endpoints as effecient
   * as possible.
   */
  void setCloseOnExec(bool enable);

  /** Tests whether the underlying system @c TCP_DEFER_ACCEPT flag is set. */
  bool deferAccept() const;

  /** Enables/disables the @c TCP_DEFER_ACCEPT flag on this connector. */
  void setDeferAccept(bool enable);

  /** Tests whether the underlying system @c TCP_QUICKACK flag is set. */
  bool quickAck() const;

  /** Enables/disables the @c TCP_QUICKACK flag on this connector. */
  void setQuickAck(bool enable);

  /** Tests whether the underlying system @c SO_REUSEPORT flag is set. */
  bool reusePort() const;

  /** Enables/disables the @c SO_REUSEPORT flag on this connector. */
  void setReusePort(bool enable);

  /** Tests whether the underlying system @c SO_REUSEADDR flag is set. */
  bool reuseAddr() const;

  /** Enables/disables the @c SO_REUSEADDR flag on this connector. */
  void setReuseAddr(bool enable);

  /**
   * Retrieves the number of maximum attempts to accept a new clients in a row.
   */
  size_t multiAcceptCount() const XZERO_NOEXCEPT;

  /**
   * Sets the number of attempts to accept a new client in a row.
   */
  void setMultiAcceptCount(size_t value) XZERO_NOEXCEPT;

  /**
   * Retrieves the timespan a connection may be idle within an I/O operation.
   */
  TimeSpan idleTimeout() const XZERO_NOEXCEPT;

  /**
   * Sets the timespan a connection may be idle within an I/O operation.
   */
  void setIdleTimeout(TimeSpan value);

  void start() override;
  bool isStarted() const XZERO_NOEXCEPT override;
  void stop() override;
  std::list<EndPoint*> connectedEndPoints() override;

 private:
  /**
   * Invoked internally by InetEndPoint to actually destroy/recycle this object.
   */
  void release(Connection* inetConnection);

  /**
   * Accepts up to exactly one new client.
   *
   * This call is blocking by default and can be changed to non-blocking
   * mode via @c setBlocking(bool).
   *
   * @see isBlocking() const
   * @see setBlocking(bool enable)
   */
  bool acceptOne();

  void onSelectable() XZERO_NOEXCEPT override;

  void bind(const IPAddress& ipaddr, int port);
  void listen(int backlog);

  /**
   * Invoked by InetEndPoint to inform its creator that it got close()'d.
   *
   * This will unlink the endpoint with this connector, and also notify the
   * endpoint's connection about the event via Connection::onClose().
   *
   * @see Connection::onClose()
   * @see ConnectionListener::onClosed(Connection*)
   */
  void onEndPointClosed(InetEndPoint* endpoint);
  friend class InetEndPoint;

 private:
  Scheduler* scheduler_;
  Selector* selector_;
  std::unique_ptr<SelectionKey> selectionKey_;

  std::list<InetEndPoint*> connectedEndPoints_;
  std::mutex mutex_;
  int socket_;
  int addressFamily_;
  int typeMask_;
  int flags_;
  bool blocking_;
  size_t backlog_;
  size_t multiAcceptCount_;
  TimeSpan idleTimeout_;
  bool isStarted_;
};

inline Scheduler* InetConnector::scheduler() const XZERO_NOEXCEPT {
  return scheduler_;
}

inline TimeSpan InetConnector::idleTimeout() const XZERO_NOEXCEPT {
  return idleTimeout_;
}

}  // namespace xzero
