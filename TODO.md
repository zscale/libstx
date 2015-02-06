## FiXME

## Incomplete TODO items

### Milestone 1

- NativeScheduler a typedef to the best OS's implementation
- linux: LinuxScheduler (using epoll, timerfd, eventfd)
- generic: SelectScheduler (using select, pipe)

- [x] make HttpFileHandler testable
- [ ] HttpFileHandler: make ETag-header generation customizable?
- [ ] write full tests for HttpFileHandler using MockTransport
- [ ] ensure 100-continue is sent async, too (maybe not needed)
- [ ] timeout management (ideally testable)
- [ ] rework exception management
- [ ] how to handle accept() errors (i.e. in InetConnector)?

### Milestone 2

- [ ] fcgi transport
- [ ] SSL support, `SslConnector` & `SslEndPoint` chaining to real
      connector/endpoint.
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
