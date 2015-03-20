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

DatagramHandler DatagramConnector::handler() const {
  return handler_;
}

void DatagramConnector::setHandler(DatagramHandler handler) {
  handler_ = handler;
}

} // namespace xzero
