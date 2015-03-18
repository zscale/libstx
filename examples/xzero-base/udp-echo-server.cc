#include <xzero-base/net/UdpConnector.h>
#include <xzero-base/net/UdpEndPoint.h>
#include <xzero-base/executor/NativeScheduler.h>
#include <xzero-base/executor/DirectExecutor.h>
#include <xzero-base/cli/Flags.h>
#include <xzero-base/cli/CLI.h>
#include <xzero-base/WallClock.h>
#include <xzero-base/logging.h>

using namespace xzero;

int main(int argc, const char* argv[]) {
  xzero::DirectExecutor executor(false);
  xzero::NativeScheduler scheduler;

  DatagramHandler handler = [&](RefPtr<DatagramEndPoint> client) {
    if (client->message() == "quit") {
      client->send("Bye bye\n");
      client->connector()->stop();
    } else {
      client->send(client->message());
    }
  };

  UdpConnector echoServer(
      "echo", handler, &executor, &scheduler,
      IPAddress("0.0.0.0"), 3333, true, false);

  echoServer.start();

  scheduler.runLoop();
}
