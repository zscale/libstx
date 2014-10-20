#pragma once

#include <xzero/Api.h>
#include <xzero/Buffer.h>
#include <xzero/io/FileRef.h>
#include <memory>
#include <deque>

namespace xzero {

class Buffer;
class BufferRef;
class FileRef;
class EndPoint;

/**
 * Composable EndPoint Writer API.
 *
 * @todo 2 consecutive buffer writes should merge.
 * @todo consider managing its own BufferPool
 */
class XZERO_API EndPointWriter {
 public:
  EndPointWriter();
  ~EndPointWriter();

  /**
   * Writes given @p data into the chunk queue.
   */
  void write(const BufferRef& data);

  /**
   * Appends given @p data into the chunk queue.
   */
  void write(Buffer&& data);

  /**
   * Appends given chunk represented by given file descriptor and range.
   *
   * @param file file ref to read from.
   */
  void write(FileRef&& file);

  /**
   * Transfers as much data as possible into the given EndPoint @p sink.
   *
   * @retval true all data has been transferred.
   * @retval false data transfer incomplete and data is pending.
   */
  bool flush(EndPoint* sink);

 private:
  class Chunk;
  class BufferChunk;
  class BufferRefChunk;
  class FileChunk;

  std::deque<std::unique_ptr<Chunk>> chunks_;
};

// {{{ Chunk API
class XZERO_API EndPointWriter::Chunk {
 public:
  virtual ~Chunk() {}

  virtual bool transferTo(EndPoint* sink) = 0;
};

class XZERO_API EndPointWriter::BufferChunk : public Chunk {
 public:
  explicit BufferChunk(Buffer&& buffer)
      : data_(std::forward<Buffer>(buffer)), offset_(0) {}

  explicit BufferChunk(const BufferRef& buffer)
      : data_(buffer), offset_(0) {}

  explicit BufferChunk(const Buffer& copy)
      : data_(copy), offset_(0) {}

  bool transferTo(EndPoint* sink) override;

 private:
  Buffer data_;
  size_t offset_;
};

class XZERO_API EndPointWriter::BufferRefChunk : public Chunk {
 public:
  explicit BufferRefChunk(const BufferRef& buffer)
      : data_(buffer), offset_(0) {}

  bool transferTo(EndPoint* sink) override;

 private:
  BufferRef data_;
  size_t offset_;
};

class XZERO_API EndPointWriter::FileChunk : public Chunk {
 public:
  explicit FileChunk(FileRef&& ref)
      : file_(std::forward<FileRef>(ref)) {}

  ~FileChunk();

  bool transferTo(EndPoint* sink) override;

 private:
  FileRef file_;
};
// }}}

} // namespace xzero
