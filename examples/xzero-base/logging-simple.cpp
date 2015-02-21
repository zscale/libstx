#include <xzero-base/logging/LogAggregator.h>
#include <xzero-base/logging.h>

int main() {
  xzero::LogAggregator::get().setLogLevel(xzero::LogLevel::trace);
  xzero::logDebug("example.logging-example", "Hello, %s", "World");
}
