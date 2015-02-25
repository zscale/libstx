#include <xzero-base/net/DnsClient.h>
#include <xzero-base/net/IPAddress.h>
#include <xzero-base/RuntimeError.h>
#include <vector>
#include <netdb.h>

namespace xzero {

// TODO: prefer getnameinfo as gethostbyname() is not IPv6-friendly

std::vector<IPAddress> DnsClient::resolveAll(const std::string& name) {
  std::lock_guard<std::mutex> _lk(cacheMutex_);
  auto i = cache_.find(name);
  if (i != cache_.end())
    return i->second;

  hostent* he = gethostbyname(name.c_str());
  if (he == nullptr)
    throw RUNTIME_ERROR(hstrerror(h_errno));

  std::vector<IPAddress> list;
  for (int i = 0; he->h_addr_list[i]; ++i) {
    switch (he->h_addrtype) {
      case AF_INET:
        list.push_back(IPAddress((in_addr*) he->h_addr_list[i]));
        break;
      case AF_INET6:
        list.push_back(IPAddress((in6_addr*) he->h_addr_list[i]));
        break;
      default:
        break;
    }
  }

  return cache_[name] = list;
}

IPAddress DnsClient::resolve(const std::string& name) {
  std::lock_guard<std::mutex> _lk(cacheMutex_);
  auto i = cache_.find(name);
  if (i != cache_.end())
    return i->second[0];

  hostent* he = gethostbyname(name.c_str());
  if (he == nullptr)
    throw RUNTIME_ERROR(hstrerror(h_errno));

  std::vector<IPAddress> list;
  for (int i = 0; he->h_addr_list[i]; ++i) {
    switch (he->h_addrtype) {
      case AF_INET:
        list.push_back(IPAddress((in_addr*) he->h_addr_list[i]));
        break;
      case AF_INET6:
        list.push_back(IPAddress((in6_addr*) he->h_addr_list[i]));
        break;
      default:
        break;
    }
  }

  cache_[name] = list;

  return list[0];
}

void DnsClient::clearCache() {
  std::lock_guard<std::mutex> _lk(cacheMutex_);
  cache_.clear();
}

}  // namespace xzero
