#include <xzero-base/net/UdpConnector.h>
#include <xzero-base/net/UdpEndPoint.h>
#include <xzero-base/net/LocalDatagramConnector.h>
#include <xzero-base/net/LocalDatagramEndPoint.h>
#include <xzero-base/executor/NativeScheduler.h>
#include <xzero-base/executor/DirectExecutor.h>
#include <xzero-base/logging.h>

using namespace xzero;

void playAround(LocalDatagramConnector& connector) {
  auto ep = connector.send(BufferRef("Hello, World"));
  logInfo("playground", "Received %zu response(s).", ep->responses().size());

  // Here we assume, that the above message is processed immediately.

  for (size_t i = 0; i < ep->responses().size(); ++i) {
    logInfo("playground", "response[%zu]: %s",
            i, ep->responses()[i].c_str());
  }
}

int main(int argc, const char* argv[]) {
  xzero::DirectExecutor executor(false);
  xzero::NativeScheduler scheduler;

  DatagramHandler handler = [&](RefPtr<DatagramEndPoint> client) {
    if (client->message() == "quit") {
      client->send("Bye bye\n");
      client->connector()->stop();
    } else {
      logInfo("echo", "Received message: \"%*s\"",
              client->message().size(),
              client->message().data());
      client->send(client->message());
    }
  };

  UdpConnector udpEcho(
      "echo", handler, &executor, &scheduler,
      IPAddress("::"), 3333, true, false);

  LocalDatagramConnector localEcho("echo-local", handler, &executor);

  udpEcho.start();
  localEcho.start();

  playAround(localEcho);

  scheduler.runLoop();
}
