/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "stx/exception.h"
#include "stx/http/httpconnectionpool.h"

namespace stx {
namespace http {

HTTPConnectionPool::HTTPConnectionPool(
    stx::TaskScheduler* scheduler,
    HTTPClientStats* stats) :
    scheduler_(scheduler),
    stats_(stats) {}

Future<HTTPResponse> HTTPConnectionPool::executeRequest(
    const HTTPRequest& req) {
  return executeRequest(
      req,
      [] (Promise<HTTPResponse> promise) {
        return new HTTPResponseFuture(promise);
      });
}

Future<HTTPResponse> HTTPConnectionPool::executeRequest(
    const HTTPRequest& req,
    const stx::InetAddr& addr) {
  return executeRequest(
      req,
      addr,
      [] (Promise<HTTPResponse> promise) {
        return new HTTPResponseFuture(promise);
      });
}

Future<HTTPResponse> HTTPConnectionPool::executeRequest(
    const HTTPRequest& req,
    Function<HTTPResponseFuture* (Promise<HTTPResponse> promise)> factory) {
  if (!req.hasHeader("Host")) {
    RAISE(kRuntimeError, "missing Host header");
  }

  auto addr = dns_cache_.resolve(req.getHeader("Host"));
  if (!addr.hasPort()) {
    addr.setPort(80);
  }

  return executeRequest(req, addr, factory);
}

Future<HTTPResponse> HTTPConnectionPool::executeRequest(
    const HTTPRequest& req,
    const stx::InetAddr& addr,
    Function<HTTPResponseFuture* (Promise<HTTPResponse> promise)> factory) {
  Promise<HTTPResponse> promise;

  leaseConnection(
      addr,
      promise,
      [this, req, promise, factory] (HTTPClientConnection* conn) {
    auto http_future = factory(promise);

    try {
      conn->executeRequest(req, http_future);
    } catch (const std::exception& e) {
      http_future->onError(e);
    }
  });

  return promise.future();
}

void HTTPConnectionPool::parkConnection(
    HTTPClientConnection* conn,
    InetAddr addr) {
  if (conn->isIdle()) {
    std::unique_lock<std::mutex> l(connection_cache_mutex_);
    connection_cache_.emplace(addr.ipAndPort(), conn);
  } else {
    delete conn;
  }
}

void HTTPConnectionPool::leaseConnection(
    const stx::InetAddr& addr,
    Promise<HTTPResponse> promise,
    Function<void (HTTPClientConnection* conn)> callback) {
  std::unique_lock<std::mutex> lk(connection_cache_mutex_);
  auto iter = connection_cache_.find(addr.ipAndPort());

  if (iter != connection_cache_.end()) {
    ScopedPtr<HTTPClientConnection> conn(iter->second);
    connection_cache_.erase(iter);
    lk.unlock();

    scheduler_->runOnNextWakeup(
        std::bind(&HTTPConnectionPool::parkConnection, this, conn.get(), addr),
        conn->onReady());

    callback(conn.release());
  } else {
    lk.unlock();

    try {
      net::TCPConnection::connectAsync(
          addr,
          scheduler_,
          [this, promise, callback, addr] (
              ScopedPtr<net::TCPConnection> tcp_conn) mutable {
            try {
              tcp_conn->checkErrors();

              ScopedPtr<HTTPClientConnection> conn(
                  new HTTPClientConnection(
                      std::move(tcp_conn),
                      scheduler_,
                      stats_));

              scheduler_->runOnNextWakeup(
                  std::bind(
                      &HTTPConnectionPool::parkConnection,
                      this,
                      conn.get(),
                      addr),
                  conn->onReady());

              callback(conn.release());
            } catch (const std::exception& e) {
              promise.failure(e);
            }
          });
    } catch (const std::exception& e) {
      promise.failure(e);
    }
  }
}

HTTPClientStats* HTTPConnectionPool::stats() {
  return stats_;
}

}
}
