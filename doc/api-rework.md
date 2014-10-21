
## Decoupling Attempt

The following other components need to be accessable ...

### HttpRequest

- current datetime: connection.worker().now()
- connection.state()
- connection abort: connection.abort()
- posting into the http i/o thread: connection.post()
- request duration: now() - timeStart()
- logging: connection.log()
- connection's remote|local ip|port
- access to thread-local HttpFileMgr in updatePathInfo()
- upon response serialization
  - onPostProcess hook: connection.worker.server.onPostProcess
  - connection.shouldKeepAlive
  - connection.worker.server.maxKeepAlive
  - connection.worker.server.maxKeepAliveRequests
  - connection.worker.server.advertise
  - connection.worker.server.tag
  - connection.worker.server.tcpCork
  - connection.socket.setTcpCork
- connection.isOpen
- connection.setState
- connection.write
- connection.isOutputPending
- connection.ref | unref
- inside finalize()
  - connection.worker.server.onRequestDone()
  - connection.clearRequestBody()
  - connection.resume()
  - connection.close()
- connection.clientAbortHandler
- connection.wantRead

28 deps

### HttpConnection

- worker.post() -- posting into the http i/o thread
- worker.server.requestHeaderBufferSize
- worker.server.requestBodyBufferSize
- worker.server.onConnectionClose()
- worker.release(HttpConnection self)
- worker.server.tcpNoDelay
- worker.server.lingering
- worker.server.onConnectionOpen()
- worker.server.maxReadIdle
- worker.server.maxWriteIdle
- worker.server.maxKeepAlive
- worker.now()
- worker.server.maxRequestUriSize
- worker.server.maxRequestHeaderSize
- worker.server.maxRequestHeaderCount
- worker.server.maxRequestBodySize
- worker.server.requestHeaderBufferSize
- worker.server.requestBodyBufferSize
- worker.handleRequest()
- worker.server.onRequestDone()
- worker.server.onConnectionStateChanged()
- worker.log()

21 deps

### HttpWorker

- server.currentWorker()
- server.onWorkerSpawn()
- server.onWorkerUnspawn()
- server.log()
- in handleRequest:
  - server.onPreProcess()
  - server.requestHandler()

6 deps

### HttpServer



### Thread-Aware I/O Synchronization Points

```

HttpConnection.onOpen:
  HttpConnection.wantFill:
    EndPoint.wantFill()
      HttpConnection.onFillable()
        HttpParser.parseFragment()
          HttpChannel.onMessageHeaderEnd()
            HttpChannel.handleRequest()...
              HttpConnection.wantFlush()

HttpConnection.wantFlush:
  EndPoint.wantFlush()
    HttpConnection.onFlushable()
      EndPointWriter.flush(endpoint)
      HttpConnection.wantFill

LifeCycle ::= idle (reading (handling writing)* completed)*

| accept()
|
\_
  | HttpConnection.onOpen
  | - wantFill
  |   - onFillable
  |     - parseFragment
  |       - HttpChannel.onMessageBegin
  |       - HttpChannel.onMessageHeader
  |       - HttpChannel.onMessageHeaderEnd
  |         - HttpChannel.handleRequest
  |           - HttpChannel.send
  |             - HttpConnection.wantFlush
  |               - HttpConnection.onFlushable
  |                 - EndPointWriter.flush
  |                 - HttpConnection.onFlushComplete
  |                 - HttpConnection.wantFlush(false)
  |                 - HttpConnection.onCompleteCallback

```



















