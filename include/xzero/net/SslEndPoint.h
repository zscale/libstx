// This file is part of the "x0" project
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <xzero/Api.h>
#include <xzero/net/EndPoint.h>
#include <xzero/executor/Scheduler.h>
#include <xzero/IdleTimeout.h>
#include <openssl/ssl.h>

namespace xzero {

class SslConnector;

/**
 * SSL EndPoint, aka SSL socket.
 */
class XZERO_API SslEndPoint : public EndPoint {
 public:
  SslEndPoint(int socket, SslConnector* connector, Scheduler* scheduler);
  ~SslEndPoint();

  int handle() const noexcept { return handle_; }

  bool isOpen() const override;
  void close() override;

  /**
   * Closes the connection the hard way, by ignoring the SSL layer.
   */
  void abort();

  /**
   * Reads from remote endpoint and fills given buffer with it.
   */
  size_t fill(Buffer* sink) override;

  /**
   * Appends given buffer into the pending buffer vector and attempts to flush.
   */
  size_t flush(const BufferRef& source) override;
  size_t flush(int fd, off_t offset, size_t size) override;

  /**
   * Ensures that the SSL socket is ready for receiving data.
   *
   * Any pending data in the fillBuffer will preceed.
   *
   * This might internally cause write <b>and</b> read operations
   * through the SSL layer.
   */
  void wantFill() override;

  /** Ensures that the SSL socket is ready for more flush operations.
   *
   * Any pending data in the flushBuffer will be sent before
   * @c connection()->onFlushable() will be invoked.
   *
   * This might internally cause write <b>and</b> read operations
   * through the SSL layer.
   */
  void wantFlush() override;

  TimeSpan idleTimeout() override;
  void setIdleTimeout(TimeSpan timeout) override;
  bool isBlocking() const override;
  void setBlocking(bool enable) override;
  bool isCorking() const override;
  void setCorking(bool enable) override;
  std::string toString() const override;

  /**
   * Retrieves the string that is identifies the negotiated next protocol, such
   * as "HTTP/1.1" or "SPDY/3.1".
   *
   * This method is implemented using NPN or ALPN protocol extensions to TLS.
   */
  BufferRef nextProtocolNegotiated() const;

 private:
  void onHandshake();
  void fillable();
  void flushable();
  void shutdown();
  void onTimeout();

  friend class SslConnector;

  enum class Desire { None, Read, Write };

  static void tlsext_debug_cb(
      SSL* ssl, int client_server, int type,
      unsigned char* data, int len, SslEndPoint* self);

 private:
  int handle_;
  bool isCorking_;
  SslConnector* connector_;
  Scheduler* scheduler_;
  SSL* ssl_;
  Desire bioDesire_;
  Scheduler::HandleRef io_;
  IdleTimeout idleTimeout_;
};

} // namespace xzero
