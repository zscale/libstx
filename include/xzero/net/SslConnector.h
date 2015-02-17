// This file is part of the "x0" project
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <xzero/Api.h>
#include <xzero/net/InetConnector.h>
#include <xzero/net/SslEndPoint.h>
#include <list>
#include <memory>
#include <openssl/ssl.h>

namespace xzero {

class SslContext;

/**
 * SSL Connector.
 *
 * @see InetConnector
 * @see SslEndPoint
 */
class XZERO_API SslConnector : public InetConnector {
 public:
  /**
   * Initializes this connector.
   *
   * @param name Describing name for this connector.
   * @param executor Executor service to run handlers on
   * @param scheduler Scheduler service to use for scheduling tasks
   * @param clock Wall clock used for timeout management.
   * @param idleTimeout timespan indicating how long a connection may be idle.
   * @param eh exception handler for errors in hooks or during events.
   * @param ipaddress TCP/IP address to listen on
   * @param port TCP/IP port number to listen on
   * @param backlog TCP backlog for this listener.
   * @param reuseAddr Flag indicating @c SO_REUSEADDR.
   * @param reusePort Flag indicating @c SO_REUSEPORT.
   *
   * @throw std::runtime_error on any kind of runtime error.
   */
  SslConnector(const std::string& name, Executor* executor,
               Scheduler* scheduler, WallClock* clock, TimeSpan idleTimeout,
               std::function<void(const std::exception&)> eh,
               const IPAddress& ipaddress, int port, int backlog,
               bool reuseAddr, bool reusePort);
  ~SslConnector();

  /**
   * Adds a new SSL context (certificate & key) pair.
   *
   * You must at least add one SSL certificate. Adding more will implicitely
   * enable SNI support.
   */
  void addContext(const std::string& crtFilePath,
                  const std::string& keyFilePath);

  void start() override;
  bool isStarted() const XZERO_NOEXCEPT override;
  void stop() override;
  std::list<RefPtr<EndPoint>> connectedEndPoints() override;

  RefPtr<EndPoint> createEndPoint(int cfd) override;
  void onEndPointCreated(const RefPtr<EndPoint>& endpoint) override;

  SslContext* selectContext(const char* servername) const;

 private:
  SslContext* defaultContext() const;

  static int selectContext(SSL* ssl, int* ad, SslConnector* connector);

  friend class SslEndPoint;
  friend class SslContext;

 private:
  std::list<std::unique_ptr<SslContext>> contexts_;
};

inline SslContext* SslConnector::defaultContext() const {
  return !contexts_.empty()
      ? contexts_.front().get()
      : nullptr;
}

} // namespace xzero


