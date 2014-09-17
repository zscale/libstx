#pragma once

#include <xzero/Api.h>
#include <xzero/DateTime.h>
#include <memory>
#include <list>

namespace xzero {

class Connector;
class EndPoint;
class ConnectionListener;

/**
 * A Connection is responsible for processing an EndPoint.
 *
 * A Connection derivate can implement various stream oriented protocols.
 * This doesn't necessarily has to be HTTP, but can also be SMTP or anything
 * else.
 */
class XZERO_API Connection {
 public:
  explicit Connection(std::shared_ptr<EndPoint> endpoint);
  virtual ~Connection();

  /**
   * Callback, invoked when connection was opened.
   */
  virtual void onOpen();

  /**
   * Callback, invoked when connection is closed.
   */
  virtual void onClose();

  /**
   * Retrieves the corresponding endpoint for this connection.
   */
  EndPoint* endpoint() const;

  /**
   * Registers given @p listener to this connection.
   *
   * @param listener the listener to register.
   *
   * @see void Connector::addListener(ConnectionListener* listener)
   */
  void addListener(ConnectionListener* listener);

  /**
   * Closes the underlying endpoint.
   */
  virtual void close();

  virtual size_t getMessagesIn() const;
  virtual size_t getMessagesOut() const;
  virtual size_t getBytesIn() const;
  virtual size_t getBytesOut() const;

  DateTime creationTime() const noexcept { return creationTime_; }
  void setCreationTime(DateTime dt) noexcept { creationTime_ = dt; }

  /**
   * Configures the input buffer size for this Connection.
   *
   * @param size number of bytes the input buffer for this connection may use.
   */
  virtual void setInputBufferSize(size_t size);

  /**
   * Ensures onFillable() is invoked when data is available for
   * read.
   *
   * In any case of an error, onInterestFailure(const std::exception&) is invoked.
   */
  void wantFill();

  /**
   * Ensures onFlushable() is invoked when underlying endpoint is ready
   * to write.
   *
   * In any case of an error, onInterestFailure(const std::exception&) is invoked.
   */
  void wantFlush();

  /**
   * Event callback being invoked when data is available for read.
   */
  virtual void onFillable() = 0;

  /**
   * Event callback being invoked when underlying endpoint ready for write.
   */
  virtual void onFlushable() = 0;

  /**
   * Event callback being invoked on any errors while waiting for data.
   *
   * For example read timeout (or possibly connection timeout?).
   */
  virtual void onInterestFailure(const std::exception& error);

  /**
   * Callback being invoked when a read-timeout has been reached.
   *
   * @retval true close the endpoint.
   * @retval false ignore the timeout, do not close.
   *
   * By default the implementation of Connection::onReadTimeout() returns just
   * true.
   */
  virtual bool onReadTimeout();

 private:
  std::shared_ptr<EndPoint> endpoint_;
  DateTime creationTime_;
  std::list<ConnectionListener*> listeners_;
};

inline EndPoint* Connection::endpoint() const {
  return endpoint_.get();
}

/**
 * Connection event listener, such as on-open and on-close events.
 *
 * @see void Connector::addListener(ConnectionListener* listener)
 */
class XZERO_API ConnectionListener {
 public:
  virtual ~ConnectionListener();

  /** Invoked via Conection::onOpen(). */
  virtual void onOpened(Connection* connection);

  /** Invoked via Conection::onClose(). */
  virtual void onClosed(Connection* connection);
};

} // namespace xzero
