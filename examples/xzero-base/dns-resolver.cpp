// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/net/DnsClient.h>

using namespace xzero;

int main(int argc, const char* argv[]) {
  DnsClient dns;

  if (argc == 1) {
    static const char* debuggingArgs[] = {
      argv[0],
      "ipv6.google.com",
      nullptr,
    };
    argc = 2;
    argv = debuggingArgs;
  }

  for (int i = 1; i < argc; ++i) {
    printf("resolving '%s'\n", argv[i]);
    auto ips = dns.resolveAll(argv[i]);
    printf("  found IPs: %zu\n", ips.size());

    for (const IPAddress& ip: ips)
      printf("    ip: %s\n", ip.c_str());
  }

  return 0;
}
