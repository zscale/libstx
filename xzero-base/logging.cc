#include <xzero-base/logging.h>
#include <xzero-base/logging/LogAggregator.h>
#include <xzero-base/logging/LogSource.h>
#include <xzero-base/RuntimeError.h>

namespace xzero {

void logError(const char* component, const std::exception& e) {
  LogSource* logger = LogAggregator::get().getSource(component);
  logger->error(e);
}

} // namespace xzero
