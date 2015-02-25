#include <xzero-base/net/DnsClient.h>
#include <xzero-base/net/IPAddress.h>
#include <xzero-base/RuntimeError.h>
#include <vector>
#include <netdb.h>

namespace xzero {

std::vector<IPAddress> DnsClient::resolveAll(const std::string& name) {
  std::lock_guard<std::mutex> _lk(cacheMutex_);
  auto i = cache_.find(name);
  if (i != cache_.end())
    return i->second;

  // TODO: use gethostbyname_r() if available

  hostent* he = gethostbyname(name.c_str());
  if (he == nullptr)
    throw RUNTIME_ERROR(hstrerror(h_errno));

  std::vector<IPAddress> list;
  for (int i = 0; he->h_addr_list[i]; ++i)
    list.push_back(IPAddress(he->h_addr_list[i]));

  return cache_[name] = list;
}

IPAddress DnsClient::resolve(const std::string& name) {
  std::lock_guard<std::mutex> _lk(cacheMutex_);
  auto i = cache_.find(name);
  if (i != cache_.end())
    return i->second[0];

  // TODO: use gethostbyname_r() if available

  hostent* he = gethostbyname(name.c_str());
  if (he == nullptr)
    throw RUNTIME_ERROR(hstrerror(h_errno));

  std::vector<IPAddress> list;
  for (int i = 0; he->h_addr_list[i]; ++i)
    list.push_back(IPAddress(he->h_addr_list[i]));

  cache_[name] = list;

  return list[0];
}

void DnsClient::clearCache() {
  std::lock_guard<std::mutex> _lk(cacheMutex_);
  cache_.clear();
}

}  // namespace xzero
