 
### Response

- https://gist.github.com/paulasmuth/5d777701c8eff8826e68

### converge

- [ ] io: File (is more a FileStream / FileChannel than a File [Info])
  - got replaced via std::{i,o}stream
  - fixme: conflicts with fnord's InputStream/OutputStream - why?
  - missing: access to fd
  - missing: clone()
  - missing: truncate()
  - missing: Buffer integration
  - missing: some open flags: AutoDelete | AllowFork | Trunc | Create[OnOpen]
- [ ] EH: global exception handlers
- [ ] Assets (MemoryFile) example
- [ ] UDP networking
- [ ] StatsServer as seperate xzero-stats module
- [ ] introspection: `std::string inspect(const TYPE&)`
- [ ] Application
- [ ] IEEE754
- [ ] traits
- [ ] threading
- [ ] util/BinaryMessageWriter
- [ ] util/BinaryMessageReader
- [ ] reflection
- [ ] Status (required by rps / json-rpc)
- [ ] rpc
- [ ] json-rpc
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

- [ ] fnord-chart: should be dedicated library, it's too specific?
- [ ] fnord-metricdb: should be dedicated library?
- [ ] fnord-sstable: should be dedicated library?
- [ ] fnord-webcomponents: should be dedicated library?

