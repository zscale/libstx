#include <xzero-base/io/MemoryMap.h>
#include <xzero-base/RuntimeError.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

namespace xzero {

MemoryMap::MemoryMap(int fd, off_t ofs, size_t size, bool rw)
    : data_(nullptr),
      size_(0),
      mode_(0) {

  int prot = rw ? PROT_READ | PROT_WRITE : PROT_READ;
  data_ = mmap(nullptr, size, prot, MAP_SHARED, fd, ofs);
  if (!data_)
    RAISE_ERRNO(errno);

  size_ = size;
  mode_ = prot;
}

MemoryMap::MemoryMap(MemoryMap&& mm)
  : data_(mm.data_),
    size_(mm.size_),
    mode_(mm.mode_) {
  mm.data_ = nullptr;
  mm.size_ = 0;
  mm.mode_ = 0;
}

MemoryMap& MemoryMap::operator=(MemoryMap&& mm) {
  if (data_)
    munmap(data_, size_);

  data_ = mm.data_;
  mm.data_ = nullptr;

  size_ = mm.size_;
  mm.size_ = 0;

  return *this;
}

MemoryMap::~MemoryMap() {
  if (data_) {
    munmap(data_, size_);
  }
}

bool MemoryMap::isReadable() const {
  return mode_ & PROT_READ;
}

bool MemoryMap::isWritable() const {
  return mode_ & PROT_WRITE;
}

}  // namespace xzero

