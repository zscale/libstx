## Incomplete TODO items

### Milestone 1

- [ ] fix LocalConnector vs InetConnector inconsistencies (see utests wrt keep-alive)
- [x] http1: properly handle Connection header tokens.
- [x] http1: proper protocol error logging
- [ ] net: EndPoint timeout management
  - read timeouts should respond 408
  - write timeouts should abort() the connection
  - keepalive timeouts should close() the connection
- [x] properly handle uncaught exceptions in http connector
- [x] logging api
- [x] implement InetEndPoint::localAddr / remoteAddr
- [x] fully support GET/HEAD on static files (including client cache awareness, range requests).
- [x] request body usage example
- [x] support `Expect: 100-continue`
- [x] write a demo http server showcasing exceptions
- [x] high level Servlet API
  - [x] auto-responds on `Expect: 100-continue`
  - [x] auto-consumes request body into a memory buffer or file buffer

### Milestone 2

- [ ] `InetEndPoint::wantFill()` to honor `TCP_DEFER_ACCEPT`
- [ ] UdpConnector
- [ ] SSL support, `SslConnector` & `SslEndPoint` chaining to real
      connector/endpoint.
