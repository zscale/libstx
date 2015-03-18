#include <xzero-base/net/UdpConnector.h>
#include <xzero-base/net/UdpEndPoint.h>
#include <xzero-base/executor/NativeScheduler.h>
#include <xzero-base/executor/DirectExecutor.h>
#include <xzero-base/cli/Flags.h>
#include <xzero-base/cli/CLI.h>
#include <xzero-base/WallClock.h>
#include <xzero-base/logging.h>

using namespace xzero;

void onEcho(RefPtr<DatagramEndPoint> client) {
  logInfo("echo", "client said: \"%*s\"",
                  client->message().size(),
                  client->message().data());

  client->reply(client->message());
}

int main(int argc, const char* argv[]) {
  xzero::DirectExecutor executor(false);
  xzero::NativeScheduler scheduler;

  DatagramHandler handler;

  UdpConnector srv("echo", handler, &executor, &scheduler,
      IPAddress("0.0.0.0"), 3333, true, false);

  srv.start();

  scheduler.runLoop();
}
