## Incomplete TODO items

### Milestone 1

- [ ] test: call completed() before contentLength is satisfied in non-chunked mode (shall be transport generic)
- [ ] test: attempt to write more data than contentLength in non-chunked mode (shall be transport generic)
- [ ] fix threaded http segv
- [ ] net: EndPoint timeout management
- [ ] `HttpTransport::onInterestFailure()` => `(factory || connector)->report(this, error);`
- [ ] evaluate the need of request lifecycle hooks
  - onConnectionOpen (diag)
  - onRequestPreHandle (diag)
  - onResponsePreCommit (dynamic output compression)
  - onResponsePostCommit (diag)
  - onResponseComplete (accesslog)
  - onConnectionClose (diag)

### Milestone 2

- [ ] `InetEndPoint::wantFill()` to honor `TCP_DEFER_ACCEPT`
- [ ] SSL support, `SslConnector` & `SslEndPoint` chaining to real
      connector/endpoint.
- [ ] UdpConnector
