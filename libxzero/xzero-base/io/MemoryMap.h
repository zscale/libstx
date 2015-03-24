// This file is part of the "libxzero" project
//   (c) 2009-2015 Christian Parpart <https://github.com/christianparpart>
//   (c) 2014-2015 Paul Asmuth <https://github.com/paulasmuth>
//
// libxzero is free software: you can redistribute it and/or modify it under
// the terms of the GNU Affero General Public License v3.0.
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once
#include <xzero-base/Api.h>

namespace xzero {

class XZERO_API MemoryMap {
 public:
  MemoryMap(int fd, off_t ofs, size_t size, bool rw);
  MemoryMap(MemoryMap&& mm);
  MemoryMap(const MemoryMap& mm) = delete;
  MemoryMap& operator=(MemoryMap&& mm);
  MemoryMap& operator=(MemoryMap& mm) = delete;
  ~MemoryMap();

  bool isReadable() const;
  bool isWritable() const;

  void* data() const;
  size_t size() const;
  void* begin() const;
  void* end() const;
  template<typename T> T* structAt(size_t ofs) const;

 private:
  void* data_;
  size_t size_;
  int mode_;
};

inline void* MemoryMap::data() const {
  return data_;
}

inline size_t MemoryMap::size() const {
  return size_;
}

inline void* MemoryMap::begin() const {
  return data_;
}

inline void* MemoryMap::end() const {
  return ((char*) data_) + size_;
}

template<typename T> inline T* MemoryMap::structAt(size_t ofs) const {
  return (T*) (((char*) data_) + ofs);
}

}  // namespace xzero
