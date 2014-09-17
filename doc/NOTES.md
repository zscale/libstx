
- the connector is ensuring blocking or nonblocking mode
- so the LocalConnector is always blocking single threaded
- & the InetConnector supports blocking and nonblocking,
  - depending on the executor passed.
  - if `dynamic_cast<EventExecutor*>(executor)`, then non-blocking single threaded
    - scheduler is set to the EventExecutor
  - if `dynamic_cast<ThreadPool*>(executor)`, then blocking threaded
    - requires some `Scheduler`

### LocalConnector

- runs modes:
  - blocking multi threaded
- `ByteArrayEndPoint`
  - onFillable: always fillable when data is available for read.
  - flush() always flushes directly.

### InetConnector

- runs modes:
  - non-blocking single threaded
  - blocking multi threaded
- requires:
  - EventExecutor: for I/O notifications
  - Scheduler: for timeout management
  - Executor: for handler execution
- `InetEndPoint`
  - manages its timeouts on flush operations
  - onFillable: only invoked when data ready to read




















