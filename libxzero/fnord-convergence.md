 
### Response

- https://gist.github.com/paulasmuth/5d777701c8eff8826e68

### converge

- [ ] VFS (bridge to FileRepository)
- [ ] WhitelistVFS (bad by design, but keep it for now)
- [ ] InetAddr: (kept for compat only)
- [ ] StatsServer as seperate xzero-stats module
- [ ] io: File (is more a FileStream / FileChannel than a File [Info])
  - got replaced via std::{i,o}stream
  - fixme: conflicts with fnord's InputStream/OutputStream - why?
  - missing: access to fd
  - missing: clone()
  - missing: truncate()
  - missing: Buffer integration
  - missing: some open flags: AutoDelete | AllowFork | Trunc | Create[OnOpen]
- [ ] thread::EventLoop (equivalent of xzero::{Posix,Native}Scheduler)
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
- [x] Future
- [x] Scheduler::runOnWakeup()-family (needed?)
- [x] thread::ThreadPool
- [x] introspection: `std::string inspect(const TYPE&)`
- [x] PageManager

### DEPENDENCIES

- rpc: base, Future, HttpClient

### concerns during converge

- ...

### concerns

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
