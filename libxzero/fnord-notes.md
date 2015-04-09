- my ThreadPool has a hard cap on thread-count, these are pre-spawned (not on demand).
  - could spawn them on-demand up to hard-cap, with MaxValue == unlimited.
- fnord::VFS is xzero::FileRepository, but WhitelistVFS is to
  dependant on local file system instead of a chained other VFS.
  - I could rename my FileRepository to VFS - does this make sense?
  - local-fs: LocalFileRepository
  - in-mem-fs: MemoryFileRepository
- VFSFile: is more like the data part and it's filesize only. dislike.
- VFS::openFile(): should not return a `RefPtr<>`
- ... 
