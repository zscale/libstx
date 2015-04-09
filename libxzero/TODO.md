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

### Usage Ideas

- FnordMetric/2
- port x0d to use this library instead
  - write a dedicated haproxy-like load balancer
- http2-to-http1 proxy
x
