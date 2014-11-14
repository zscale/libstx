
HACKING
=======

## Exception Handling

### Rules

- Ensure `noexcept` really does not raise at all.

- Every exception raised by `libxzero` is derived from `RuntimeError`.

- Uncaught exceptions within the user request handler generate an HTTP 500 if
  and only if no response generation wasn't started yet. the connection gets
  closed directly otherwise.

- Uncaught exceptions below the request handler will be handled via aborting
  (closing) the connection directly.

### `RuntimeError` API

- provides exceptions source location (filename and line number)
- optionally recording the stack trace (with resolving on demand)
  - optionally using `llvm-symbolize` to aid nice pretty printing

### Exception API

```cpp
class std::exception(what);
  class std::runtime_error(what);
    class RuntimeError(string message);
      class ProtocolError(string message);   // during client read/parse, transport-layer
      class EndPointEofError(EndPoint* ref); // can happen at read or write time
```

