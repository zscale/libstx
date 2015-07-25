
## API Design Goals

- Testability
- Composability
- Ability to support different HTTP transport protocols (HTTP/1, HTTP/2, FCGI, SPDY, ...)
- Platform Independancy (also abstracting away event loops, timers, clocks)
- Abstract away threading model
  - single threaded, non-blocking I/O
  - thread pool, non-blocking I/O
  - thread per connection, blocking

## API Layer

### Base API

- BufferRef
- Buffer
- DateTime
- Executor
  - SchedulingExecutor
  - DirectExecutor
- IdleTimeout
- IPAddress
- Logger
- Selectable
- Selector
- SelectionKey
- TimeSpan
- Tokenizer<T>
- WallClock

### Server API

- Server
- Connector
- ConnectionFactory
- Connection
- EndPoint
- EndPointWriter

### Server Support API

- ByteArrayEndPoint : EndPoint
- LocalConnector : Connector
- LocalEndPoint : ByteArrayEndPoint
- InetConnector : Connector
- InetEndPoint : EndPoint

### HTTP Core API layer

- HttpTransport : Connection
- HttpListener
- HttpChannel : HttpListener
- HttpDateGenerator
- HttpRequest
- HttpResponse
- HttpStatus
- HttpVersion

### HTTP/1 layer

- HttpConnectionFactory : ConnectionFactory
- HttpConnection : HttpTransport
- HttpParser
- HttpGenerator

### FastCGI layer

- FastCgiConnectionFactory : ConnectionFactory
- FastCgiTransport : HttpTransport

### HTTP/2 layer

- Http2ConnectionFactory : ConnectionFactory
- Http2Connection : HttpTransport
- HeaderParser
- HeaderGenerator
- FrameParser
- FrameGenerator
- ...

### Open API questions

- InputStream
  - FileInputStream (optimize file-to-socket transfer via sendfile)
  - PipeInputStream (optimize pipe-to-socket transfer via splice)
  - BufferInputStream
  - EndPointInputStream
  - CompositeInputStream
- OutputStream
  - SocketOutputStream?
  - EndPointOutputStream?

## Error Handling

- The library should be exception-safe
- Functions that are *NOT* meant to throw an exception must be marked `noexcept`.
- Make use of `unique_ptr` in local scope in order to avoid ugly try-catch cleanups.
- Prefer throwing `std::exception` over boolean return types.
- The documentation *MUST* reflect what exceptions can occur.
