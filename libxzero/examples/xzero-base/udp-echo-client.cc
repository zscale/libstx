#include <xzero-base/Application.h>
#include <xzero-base/net/UdpClient.h>
#include <xzero-base/net/IPAddress.h>
#include <xzero-base/Buffer.h>
#include <xzero-base/logging.h>

using namespace xzero;

int main(int argc, const char* argv[]) {
  Application::init();
  Application::logToStderr(LogLevel::Trace);

  UdpClient echoClient(IPAddress("127.0.0.1"), 3333);
  echoClient.send("Hello, World");

  Buffer response;
  echoClient.receive(&response);

  logInfo("echo", "received message: \"%*s\"", response.size(), response.data());
}
