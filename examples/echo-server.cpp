#include <xzero/Buffer.h>
#include <xzero/net/Server.h>
#include <xzero/net/ConnectionFactory.h>
#include <xzero/net/Connection.h>
#include <xzero/net/LocalConnector.h>
#include <xzero/net/InetConnector.h>
#include <xzero/net/IPAddress.h>
#include <xzero/executor/DirectExecutor.h>
#include <xzero/support/libev/LibevScheduler.h>
#include <xzero/support/libev/LibevSelector.h>
#include <xzero/support/libev/LibevClock.h>

using xzero::Buffer;
using xzero::IPAddress;

class EchoConnection : public xzero::Connection { // {{{
 public:
  EchoConnection(std::shared_ptr<xzero::EndPoint> endpoint,
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
                         std::shared_ptr<xzero::EndPoint> endpoint) override {
    return static_cast<EchoConnection*>(configure(
        new EchoConnection(endpoint, connector->executor()),
        connector));
    ;
  }
};
// }}}
std::unique_ptr<xzero::InetConnector> createInetConnector( // {{{
    const std::string& name, int port, xzero::Executor* executor,
    xzero::Scheduler* scheduler, xzero::Selector* selector,
    xzero::WallClock* clock) {

  std::unique_ptr<xzero::InetConnector> inetConnector(
      new xzero::InetConnector(name, executor, scheduler, selector, clock,
        xzero::TimeSpan::fromSeconds(30)));

  inetConnector->open(IPAddress("0.0.0.0"), port, 128, true, true);
  inetConnector->setBlocking(false);
  inetConnector->setCloseOnExec(true);
  inetConnector->setQuickAck(true);
  inetConnector->setDeferAccept(true);
  inetConnector->setMultiAcceptCount(1);

  return inetConnector;
}
// }}}

int main(int argc, const char* argv[]) {
  ev::dynamic_loop loop(0);
  xzero::DirectExecutor executor(false);
  xzero::support::LibevScheduler scheduler(loop);
  xzero::support::LibevSelector selector(loop);
  xzero::support::LibevClock clock(loop);
  xzero::Server server;

  auto localConnector = server.addConnector<xzero::LocalConnector>(&executor);
  localConnector->addConnectionFactory<EchoFactory>();

  auto inetConnector = createInetConnector("inet", 3000, &executor, &scheduler,
                                           &selector, &clock);
  inetConnector->addConnectionFactory<EchoFactory>();
  server.addConnector(std::move(inetConnector));

  server.start();

  auto ep = localConnector->createClient("Hello, World!\n");
  printf("result: %s", ep->output().c_str());

  loop.run();

  server.stop();

  return 0;
}
