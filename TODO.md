## Incomplete TODO items

### Milestone 1

- [ ] logging api
- [ ] proper protocol error logging / reporting
- [ ] FIXME: restore interest back to `NONE|READ` after onFlushable-completion
- [x] fully support GET/HEAD on static files (including client cache awareness, range requests).
- [x] request body usage example
- [x] support `Expect: 100-continue`
- [ ] properly handle uncaught exceptions in http connector
- [ ] write a demo http server showcasing exceptions
- [ ] EndPoint timeout management

### Milestone 2

- [ ] `InetEndPoint::wantFill()` to honor `TCP_DEFER_ACCEPT`
- [ ] UdpConnector
- [ ] SSL support, `SslConnector` & `SslEndPoint` chaining to real
      connector/endpoint.
- [ ] high level Servlet API
  - [ ] auto-responds on `Expect: 100-continue`
  - [ ] auto-consumes request body into a memory buffer or file buffer
