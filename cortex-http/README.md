# Xzero C++ HTTP Library

`libxzero-http` is a thin low-latency, scalable, and embeddable HTTP server library
written in modern C++.

### Feature Highlights

- Composable API
- HTTP/1.1, including chunked trailer and pipelining support
- Generic core HTTP API with support for: HTTP (HTTPS, FCGI, HTTP/2, ...)
- Client-cache aware and partial static file serving
- zero-copy networking optimizations
- Unit Tests

## Installation Requirements

- `libxzero-base` including its dependencies

### Renaming

```
xzero::http::http1::
  Http1Channel              | Channel
  Http1ConnectionFactory    | ConnectionFactory
  HttpConnection            | Connection
  HttpGenerator             | Generator
  HttpParser                | Parser

xzero::http::
  BadMessage                | ?
  HeaderField               | HeaderField
  HeaderFieldList           | HeaderFieldList
  HttpBufferedInput         | ?
  HttpChannel               | Channel
  HttpConnectionFactory     | ConnectionFactory
  HttpDateGenerator         | DateGenerator
  HttpFile                  | File
  HttpFileHandler           | FileHandler
  HttpHandler               | Handler
  HttpInfo                  | MessageInfo
  HttpInput                 | MessageBodyReader
  HttpInputListener         | MessageBodyListener
  HttpListener              | MessageListener
  HttpMethod                | RequestMethod
  HttpOutput                | MessageBodyWriter
  HttpOutputCompressor      | OutputCompressor
  HttpRangeDef              | RangeDef
  HttpRequest               | Request
  HttpRequestInfo           | RequestInfo
  HttpResponse              | Response
  HttpResponseInfo          | ResponseInfo
  HttpService               | Service
  HttpStatus                | StatusCode
  HttpTransport             | Transport
  HttpVersion               | Version

```

### Little Multiplex-Refactor

```
EndPoint
  ByteArrayEndPoint
  InetEndPoint
Connector
  ByteArrayConnector
  InetConnector
Connection
  Http1Connection(Connector, EndPoint) < HttpTransport
  Http2Connection < HttpTransport
  HttpFastCgiConnection
  HttpMockConnection < HttpTransport
HttpTransport
  Http1Connection < Connection
  FastCgiTransport(int rid, Flusher)
HttpChannel(Connector, EndPoint, HttpTransport)
  Http1Channel(Connector, Http1Connection)
  FastCgiChannel(Connector, EndPoint, HttpTransport)

```

