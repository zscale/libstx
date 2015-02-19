## misc.

```
libxzero/
  examples/
    xzero-$MOD/
      hello.cpp
  xzero-$MOD/
    xzero-$MOD.pc
```

## FHS

```
libxzero/
  xzero-base/          | base APIs, including networking
  xzero-http/          | HTTP APIs, generic and specific transports
  xzero-flow/          | Flow Language
  xzero-stats/         | StatsD Service
  xzero-chart/         | [paul-lib]
  xzero-metricdb/      | [paul-lib]
  xzero-sstable/       [ [paul-lib]
  xzero-webcomponents/ | [paul-lib]

xzero/
  RegExp.{h,cpp}

xzero-http/
  http1/
    Channel
  http2/
  fcgi/
  mock/
  StatusCode
  Transport
  Channel
  Request
  Response
  ...

#include <xzero-http/http1/HttpConnectionFactory.h>
xzero::http::http1::ConnectionFactory
xzero::http::StatusCode
xzero::http::Channel
xzero::http::Request
xzero::http::Response

xzero-flow/
  ast/
    ASTVisitor
    ...
  ir/
    Instr
    BasicBlock
    ...
  transform/
    ...
  vm/
    Program
    ...
  generator/
    IRGenerator/
  Type
  Token
  Lexer
  Parser


#include <xzero-flow/ir/Instr.h>
xzero::flow::ir::Instr

```

## Incomplete TODO items

### Milestone 1

- [ ] Process
- [x] SSL: basic `SslConnector` & `SslEndPoint`
- [x] SSL: finish SNI support
- [x] SSL: NPN support
- [x] SSL: ALPN support
- [x] SSL: SslConnector to select ConnectionFactory based on NPN/ALPN
- [ ] SSL: ability to setup a certificate password challenge callback
- [ ] improve timeout management (ideally testable)
- [ ] improve (debug) logging facility

### Milestone 2

- [ ] HttpFileHandler: make ETag-header generation customizable?
- [ ] write full tests for HttpFileHandler using MockTransport
- [ ] LinuxScheduler (using epoll, timerfd, eventfd)
- [ ] fcgi transport
- [ ] net: improved EndPoint timeout handling
      (distinguish between read/write/keepalive timeouts)
- [ ] evaluate the need of a connection/request lifecycle hook API
  - onConnectionOpen (diag)
  - onRequestPreHandle (diag)
  - onResponsePreCommit (dynamic output transformation)
  - onResponsePostCommit (diag)
  - onResponseComplete (accesslog)
  - onConnectionClose (diag)
- [ ] `HttpTransport::onInterestFailure()` => `(factory || connector)->report(this, error);`
- [ ] `InetEndPoint::wantFill()` to honor `TCP_DEFER_ACCEPT`
- [ ] UdpConnector
- [ ] chunked request trailer support & unit test
- [ ] doxygen: how to document a group of functions all at once (or, how to copydoc)
- [ ] test: call completed() before contentLength is satisfied in non-chunked mode (shall be transport generic)
- [ ] test: attempt to write more data than contentLength in non-chunked mode (shall be transport generic)
- [ ] merge libflow into libxzero
- [ ] merge libfnord into libxzero

### Usage Ideas

- FnordMetric/2
- port x0d to use this library instead
- write a dedicated haproxy-like load balancer
- http2-to-http1 proxy
x
