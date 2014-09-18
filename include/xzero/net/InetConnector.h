#pragma once

#include <xzero/Api.h>
#include <xzero/io/Selectable.h>
#include <xzero/net/Connector.h>
#include <xzero/net/IPAddress.h>
#include <list>
#include <deque>

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
   * @param ipaddress TCP/IP address to listen on
   * @param port TCP/IP port number to listen on
   * @param backlog TCP backlog for this listener.
   * @param reuseAddr Flag indicating @c SO_REUSEADDR.
   * @param reusePort Flag indicating @c SO_REUSEPORT.
   *
   * @throw std::runtime_error on any kind of runtime error.
   */
  InetConnector(const std::string& name, Executor* executor,
                Scheduler* scheduler, Selector* selector,
                const IPAddress& ipaddress, int port, int backlog,
                bool reuseAddr, bool reusePort);

  /**
   * Minimal initializer.
   *
   * @param name Describing name for this connector.
   * @param executor Executor service to run on
   * @param scheduler Scheduler service to use for timeout management
   * @param selector Selector service to use for I/O multiplexing
   */
  InetConnector(const std::string& name, Executor* executor,
                Scheduler* scheduler, Selector* selector);

  ~InetConnector();

  Scheduler* scheduler() const noexcept;

  Selector* selector() const noexcept override;

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
  bool isOpen() const noexcept;

  /**
   * Implicitely stops and finally closes this connnector.
   */
  void close();

  /**
   * Retrieves the underlying system socket handle.
   */
  virtual int handle() const noexcept;

  /**
   * Sets the underlying system socket handle.
   */
  void setSocket(int socket);

  size_t backlog() const noexcept;
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
  size_t multiAcceptCount() const noexcept;

  /**
   * Sets the number of attempts to accept a new client in a row.
   */
  void setMultiAcceptCount(size_t value) noexcept;

  void start() override;
  bool isStarted() const noexcept override;
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

  void onSelectable() noexcept override;

  void bind(const IPAddress& ipaddr, int port);
  void listen(int backlog);

  friend class InetEndPoint;
  void onEndPointClosed(InetEndPoint* endpoint);

 private:
  Scheduler* scheduler_;
  Selector* selector_;
  std::unique_ptr<SelectionKey> selectionKey_;

  std::list<InetEndPoint*> connectedEndPoints_;
  int socket_;
  int typeMask_;
  int flags_;
  size_t backlog_;
  size_t multiAcceptCount_;
  bool isStarted_;
};

inline Scheduler* InetConnector::scheduler() const noexcept {
  return scheduler_;
}

}  // namespace xzero
