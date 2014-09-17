## Incomplete TODO items

### Milestone 1

- [x] fully support GET/HEAD on static files (including client cache awareness, range requests).
- [x] request body usage example
- [ ] support `Expect: 100-continue`
- [ ] properly handle uncaught exceptions in http connector
- [ ] write a demo http server showcasing exceptions
- [ ] EndPoint timeout management
- [ ] proper protocol error logging / reporting

### Milestone 2

- [ ] `InetEndPoint::wantFill()` to honor `TCP_DEFER_ACCEPT`
- [ ] UdpConnector
- [ ] SSL support, `SslConnector` & `SslEndPoint` chaining to real
      connector/endpoint.
