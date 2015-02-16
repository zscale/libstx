## FiXME

- do we want to wrap all HTTP stuff into namespace `xzero::http`
- do we want to wrap all HTTP/1 protocol stuff into `xzero::http::v1`?
- or do we just want to rename some classes to `Http1Connection`, ...?

```
xzero::http         | generic HTTP API
xzero::http::v1     | transport for HTTP 0.9 to 1.1
xzero::http::v2     | transport for HTTP 2.0
xzero::http::fcgi   | transport for FastCGI transport
xzero::http::mock   | transport for abstract access (including unit tests)
```

## Incomplete TODO items

### Milestone 1

- [x] SSL: basic `SslConnector` & `SslEndPoint`
- [ ] SSL: ability to setup a certificate password challenge callback
- [ ] SSL: finish SNI support
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
