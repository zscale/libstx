
HACKING
=======

## Dead Code?
- Response.setCommitted()

## Exceptions

- provide stacktrace of exception root (as multiline string or string array)

```cpp
class std::exception(string message);
  class RuntimeError(string message);      // provide standard features (e.g. stack trace)
    class ProtocolError(string message);   // during client read/parse, transport-layer
    class EndPointEofError(EndPoint* ref); // can happen at read or write time
```




