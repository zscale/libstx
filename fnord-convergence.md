 
### Response

- https://gist.github.com/paulasmuth/5d777701c8eff8826e68

### Status vs Exceptions vs stdlib

- Status is a replica of Exception (RuntimeError) in terms of error codes
- RuntimeError should use Status's codes instead
- ideally integrated with `std::system_error`'s `std::error_category`
- `class std::error_code(int ev, std::error_category& ec)`
- `class std::error_condition(int ev, std::error_category& ec)`
- `class std::exception(const std::string& what)`
- `class std::invalid_argument()`

### Error Handling Proposal

- use `class std::error_code` to reflect the actual error code
- `enum class Status` to specify actual status codes
- those status codes get its own error category `class StatusCodeCategory`
- `class RuntimeError` now inherits from `std::system_error`
  and is used for the above
- The benifit of `RuntimeError` remains: ability to trace source file:line.

### converge

- [ ] StatsServer as seperate xzero-stats module
- [ ] introspection: `std::string inspect(const TYPE&)`
- [ ] Assets (MemoryFile) example

- [ ] io: File (is more a FileStream / FileChannel than a File [Info])
  - got replaced via std::{i,o}stream
  - fixme: conflicts with fnord's InputStream/OutputStream - why?
  - missing: access to fd
  - missing: clone()
  - missing: truncate()
  - missing: Buffer integration
  - missing: some open flags: AutoDelete | AllowFork | Trunc | Create[OnOpen]
- [ ] thread::Future, thread::PromiseState
- [ ] thread::EventLoop (equivalent of xzero::{Posix,Native}Scheduler)
- [ ] thread::ThreadPool
- [ ] PageManager
- [ ] fnord-rpc comm::ServerGroup to be part of base
      <xzero-base/comm/ServerGroup.h>
- [x] DateTime: `std::numeric_limits<DateTime>i { min, max }`
- [x] logging api
- [x] FNV
- [x] Assets: as part of HttpFileHandler
- [x] io: FileUtil
- [x] Random
- [x] timeconstants: via already existing `TimeSpan`
- [x] DnsCache: as DnsClient
- [x] flagparser as posix/getopt compliant option parser
- [x] string formatting: `StringUtil::hexPrint()` as `Buffer*::hexdump()`
- [x] WallClock to provide high resolution timings.
- [x] StringUtil (except hexPrint, b/c it's in BufferRef already)
- [x] Exception
- [x] BufferUtil
- [x] EH: log-and-pass (logAndPass)
- [x] EH: log-and-abort (logAndAbort)
- [x] HttpMemoryFile unit test
- [x] io: FileRepository
- [x] MappedFile
- [x] io: FileRef (got deprecated)
- [x] FreeOnDestroy: (maybe different impl via `unique_ptr` and custom deleter?)
- [x] URI as Uri
- [x] Application
- [x] thread::SignalHandler
- [x] thread::Queue
- [x] thread::Wakeup
- [x] reflection
- [x] traits
- [x] EH: global exception handlers
- [x] util/BinaryMessageWriter
- [x] util/BinaryMessageReader
- [x] Status (required by rpc / json-rpc)
      (implemented via `std::error_category`)
- [x] UDP networking
- [x] IEEE754

### DEPENDENCIES

- rpc: base, Future, HttpClient

### concerns during converge

- ...

### concerns

- Status: clarify again the use, and see if we can get it better (instead of
    blindingly migrating)
- `fnord::hash<>` why?
- InputStream, RewindableInputStream, FileInputStream, StringInputStream,
  BufferInputStream - why not std::istream?
- OutputStream, FileOutputStream, StringOutputStream, BufferOutputStream
  why not std::ostream?
- PageManager: why? contains fixme's.
  Are you willing to maintain for example PageManager in case of proj merge?

### Other Modules TBD

- [ ] fnord-chart:
- [ ] fnord-metricdb:
- [ ] fnord-sstable:
- [ ] fnord-webcomponents:
- [ ] xzero-redis: redis client support
- [ ] rpc
- [ ] json-rpc

### Ideas

- [ ] xzero-redis: redis server transport implementation;
      no actual commands, just the transport,
      so apps can use it as protocol layer.
- [ ] ...
