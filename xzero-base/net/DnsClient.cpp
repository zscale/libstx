#include <xzero-base/net/DnsClient.h>
#include <xzero-base/net/IPAddress.h>
#include <xzero-base/RuntimeError.h>
#include <vector>
#include <netdb.h>

namespace xzero {

std::vector<IPAddress> DnsClient::resolveAll(const std::string& name, int ipv) {
  std::lock_guard<std::mutex> _lk(cacheMutex_);
  auto i = cache_.find(name);
  if (i != cache_.end()) {
    if (ipv) {
      return filter(i->second, ipv);
    } else {
      return i->second;
    }
  }

  addrinfo* res = nullptr;
  addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = AI_NUMERICSERV;
  hints.ai_family = AF_UNSPEC; // any
  hints.ai_socktype = SOCK_STREAM; // any, actually

  int rc = getaddrinfo(name.c_str(), nullptr, &hints, &res);
  if (rc != 0)
    throw RUNTIME_ERROR(gai_strerror(rc));

  std::vector<IPAddress> list;

  for (addrinfo* ri = res; ri != nullptr; ri = ri->ai_next) {
    switch (ri->ai_family) {
      case AF_INET:
        list.push_back(IPAddress((sockaddr_in*) ri->ai_addr));
        break;
      case AF_INET6:
        list.push_back(IPAddress((sockaddr_in6*) ri->ai_addr));
        break;
      default:
        break;
    }
  }

  if (ipv) {
    return filter(cache_[name] = list, ipv);
  } else {
    return cache_[name] = list;
  }
}

std::vector<IPAddress> DnsClient::resolveAll(const std::string& name) {
  return resolveAll(name, 0);
}

IPAddress DnsClient::resolve(const std::string& name, int ipv) {
  // TODO: optimize for speed
  {
    std::lock_guard<std::mutex> _lk(cacheMutex_);
    auto i = cache_.find(name);
    if (i != cache_.end()) {
      if (ipv) {
        return filter(i->second, ipv)[0];
      } else {
        return i->second[0];
      }
    }
  }
  return resolveAll(name, ipv)[0];
}

IPAddress DnsClient::resolve(const std::string& name) {
  return resolve(name, 0);
}

void DnsClient::clearCache() {
  std::lock_guard<std::mutex> _lk(cacheMutex_);
  cache_.clear();
}

std::vector<IPAddress> DnsClient::filter(
    const std::vector<IPAddress>& ips, int ipv) {
  if (!ipv)
    return ips;

  std::vector<IPAddress> result;

  for (const IPAddress& ip: ips)
    if (ip.family() == ipv)
      result.push_back(ip);

  return result;
}

}  // namespace xzero
