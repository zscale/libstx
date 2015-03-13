// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/Buffer.h>
#include <xzero-base/net/Server.h>
#include <xzero-base/net/ConnectionFactory.h>
#include <xzero-base/net/Connection.h>
#include <xzero-base/net/LocalConnector.h>
#include <xzero-base/net/InetConnector.h>
#include <xzero-base/net/SslConnector.h>
#include <xzero-base/net/IPAddress.h>
#include <xzero-base/WallClock.h>
#include <xzero-base/executor/DirectExecutor.h>
#include <xzero-base/executor/NativeScheduler.h>
#include <xzero-base/RuntimeError.h>
#include <xzero-base/logging.h>

using xzero::Buffer;
using xzero::IPAddress;

class EchoConnection : public xzero::Connection { // {{{
 public:
  EchoConnection(xzero::EndPoint* endpoint,
                 xzero::Executor* executor)
      : xzero::Connection(endpoint, executor) {
  }

  ~EchoConnection() {
  }

  void onOpen() override {
    Connection::onOpen();
    wantFill();
  }

  void onFillable() override {
    Buffer data;
    endpoint()->fill(&data);

    executor()->execute([this, data]() {
      printf("echo: %s", data.c_str());
      endpoint()->flush(data);
      close();
    });
  }

  void onFlushable() override {
    //.
  }
};
// }}}
class EchoFactory : public xzero::ConnectionFactory { // {{{
 public:
  EchoFactory() : xzero::ConnectionFactory("echo") {}

  EchoConnection* create(xzero::Connector* connector,
                         xzero::EndPoint* endpoint) override {
    return static_cast<EchoConnection*>(configure(
        new EchoConnection(endpoint, connector->executor()),
        connector));
    ;
  }
};
// }}}
std::unique_ptr<xzero::InetConnector> createInetConnector( // {{{
    const std::string& name, int port, xzero::Executor* executor,
    xzero::Scheduler* scheduler, xzero::WallClock* clock) {

  std::unique_ptr<xzero::InetConnector> inetConnector(
      new xzero::InetConnector(name, executor, scheduler, clock,
        xzero::TimeSpan::fromSeconds(30), xzero::TimeSpan::Zero,
        &xzero::logAndPass));

  inetConnector->open(IPAddress("0.0.0.0"), port, 128, true, true);
  inetConnector->setBlocking(false);
  inetConnector->setCloseOnExec(true);
  inetConnector->setQuickAck(true);
  inetConnector->setDeferAccept(true);
  inetConnector->setMultiAcceptCount(1);

  return inetConnector;
}
// }}}
std::unique_ptr<xzero::SslConnector> createSslConnector( // {{{
    const std::string& name, int port, xzero::Executor* executor,
    xzero::Scheduler* scheduler, xzero::WallClock* clock) {

  std::unique_ptr<xzero::SslConnector> connector(
      new xzero::SslConnector(name, executor, scheduler, clock,
                              xzero::TimeSpan::fromSeconds(30),
                              xzero::TimeSpan::Zero,
                              &xzero::logAndPass,
                              IPAddress("0.0.0.0"), port, 128, true, true));

  connector->addContext("../../server.crt", "../../server.key");

  return connector;
}
// }}}


int main(int argc, const char* argv[]) {
  try {
    xzero::DirectExecutor executor(false);
    xzero::NativeScheduler scheduler;
    xzero::WallClock* clock = xzero::WallClock::monotonic();
    xzero::Server server;

    auto localConnector = server.addConnector<xzero::LocalConnector>(&executor);
    localConnector->addConnectionFactory<EchoFactory>();

    auto inetConnector = createInetConnector("inet", 3000, &executor, &scheduler,
                                             clock);
    inetConnector->addConnectionFactory<EchoFactory>();
    server.addConnector(std::move(inetConnector));

    auto sslConnector = createSslConnector("ssl", 3443, &executor, &scheduler,
                                           clock);
    sslConnector->addConnectionFactory<EchoFactory>();
    server.addConnector(std::move(sslConnector));

    server.start();

    auto ep = localConnector->createClient("Hello, World!\n");
    printf("local result: %s", ep->output().c_str());

    scheduler.runLoop();

    server.stop();
  } catch (const std::exception& e) {
    xzero::logAndPass(e);
  }

  return 0;
}
