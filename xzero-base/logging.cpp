#include <xzero-base/logging.h>
#include <xzero-base/logging/LogAggregator.h>
#include <xzero-base/logging/LogSource.h>

namespace xzero {

void logDebug(const std::string& component, const std::exception& e) {
  LogAggregator::get().getSource(component).log(e);
}

} // namespace xzero
