 
### Response

- https://gist.github.com/paulasmuth/5d777701c8eff8826e68

### converge

- [ ] Exception
- [ ] ExceptionHandler
- [ ] io: File, FileRef
- [ ] io: FileRepository
- [ ] MappedFile
- [ ] UDP networking
- [ ] BufferUtil
- [ ] string formatting: `StringUtil::format()` as `std::string formatString(fmt, ...)`
- [ ] string formatting: `StringUtil::hexPrint()` as `formatHex(...)` or `hexdump()`
- [ ] string formatting: `StringUtil::join()`
- [ ] string formatting: `StringUtil::split()`
- [ ] introspection: `std::string inspect(const TYPE&)`
- [ ] URI as Uri
- [ ] Application
- [ ] IEEE754
- [ ] traits
- [ ] FreeOnDestroy: (maybe different impl via `unique_ptr` and custom deleter?)
- [x] DateTime: `std::numeric_limits<DateTime>i { min, max }`
- [x] logging api
- [x] FNV
- [x] Assets: as part of HttpFileHandler
- [x] io: FileUtil
- [x] Random
- [x] timeconstants: via already existing `TimeSpan`
- [x] DnsCache: as DnsClient
- [x] flagparser as posix/getopt compliant option parser

### concerns during converge

- Exception.ofType() macht strcmp, ist das wirklich besser als `dynamic_cast`?
  wenn ja, why?
- libfnord's `RAISE(E, ..)` wirft keine exception of type E, sondern faked den
  typeNamen, ist das wirklich good[-will-attempt]?
  * ich benutze E als eigenen typen, wie ValidationError
- 

### concerns

- formatNumberMetric should be project-local (not merged)?
- Status: clarify again the use, and see if we can get it better (instead of
    blindingly migrating)
- why those stdtypes?
- Buffer: mark/setMark, why?
- ExceptionHandler as functions, not as classes?
- fnord::hash<> why?
- InputStream, RewindableInputStream, FileInputStream, StringInputStream,
  BufferInputStream - why not std::istream?
- OutputStream, FileOutputStream, StringOutputStream, BufferOutputStream
  why not std::ostream?
- PageManager: why? contains fixme's.
  Are you willing to maintain for example PageManager in case of proj merge?
- stats / StatsServer: the right place for it?
  probably make it a `<xzero/stats/...>` sub-module
- I'd opt for google-test lib, maybe your's, but any *2* gains?
- fnord-chart: should be dedicated library, it's too specific?
- fnord-metricdb: should be dedicated library?
- fnord-sstable: should be dedicated library?
- fnord-webcomponents: should be dedicated library?

### To Be Considered

- threading
- util/BinaryMessageWriter
- util/BinaryMessageReader
- reflection
- rpc
- json-rpc

