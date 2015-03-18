#include <xzero-base/net/DatagramConnector.h>

namespace xzero {

DatagramConnector::DatagramConnector(
    const std::string& name,
    DatagramHandler handler,
    Executor* executor)
    : name_(name),
      handler_(handler),
      executor_(executor) {
}

DatagramConnector::~DatagramConnector() {
}

} // namespace xzero
