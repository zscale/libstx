#include <xzero-base/net/DnsClient.h>
#include <xzero-base/net/IPAddress.h>
#include <xzero-base/RuntimeError.h>
#include <vector>
#include <netdb.h>

namespace xzero {

const std::vector<IPAddress>& DnsClient::ipv4(const std::string& name) {
  return lookupIP<sockaddr_in, AF_INET>(name, &ipv4_, &ipv4Mutex_);
}

const std::vector<IPAddress>& DnsClient::ipv6(const std::string& name) {
  return lookupIP<sockaddr_in6, AF_INET6>(name, &ipv6_, &ipv6Mutex_);
}

std::vector<IPAddress> DnsClient::ip(const std::string& name) {
  std::vector<IPAddress> result;
  try {
    const std::vector<IPAddress>& v4 = ipv4(name);
    for (const IPAddress& ip: v4) {
      result.push_back(ip);
    }
  } catch (...) {
  }

  try {
    const std::vector<IPAddress>& v6 = ipv6(name);
    for (const IPAddress& ip: v6) {
      result.push_back(ip);
    }
  } catch (...) {
  }

  if (result.empty())
    throw RUNTIME_ERROR("Could not resolve hostname " + name + ".");

  return result;
}

template<typename InetType, const int AddressFamilty>
const std::vector<IPAddress>& DnsClient::lookupIP(
    const std::string& name,
    std::unordered_map<std::string, std::vector<IPAddress>>* cache,
    std::mutex* cacheMutex) {
  std::lock_guard<decltype(*cacheMutex)> _lk(*cacheMutex);
  auto i = cache->find(name);
  if (i != cache->end())
      return i->second;

  addrinfo* res = nullptr;
  addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = AI_NUMERICSERV;
  hints.ai_family = AddressFamilty;
  hints.ai_socktype = SOCK_STREAM; // any, actually

  int rc = getaddrinfo(name.c_str(), nullptr, &hints, &res);
  if (rc != 0)
    throw RUNTIME_ERROR(gai_strerror(rc));

  std::vector<IPAddress> list;

  for (addrinfo* ri = res; ri != nullptr; ri = ri->ai_next)
    list.emplace_back(reinterpret_cast<InetType*>(ri->ai_addr));

  return (*cache)[name] = list;
}

std::vector<std::string> DnsClient::txt(const std::string& name) {
  throw RUNTIME_ERROR("Not Implemented");
}

std::vector<std::pair<int, std::string>> DnsClient::mx(const std::string& name) {
  throw RUNTIME_ERROR("Not Implemented");
}

void DnsClient::clearIPv4() {
  std::lock_guard<std::mutex> _lk(ipv4Mutex_);
  ipv4_.clear();
}

void DnsClient::clearIPv6() {
  std::lock_guard<std::mutex> _lk(ipv6Mutex_);
  ipv6_.clear();
}

void DnsClient::clearIP() {
  clearIPv4();
  clearIPv6();
}

}  // namespace xzero
