/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <string>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stx/buffer.h>
#include <stx/exception.h>
#include <stx/io/inputstream.h>
#include <stx/io/FileInputStream.h>
#include <stx/ieee754.h>

namespace stx {

std::unique_ptr<FileInputStream> FileInputStream::openFile(
    const std::string& file_path) {
  auto fp = file_path.c_str();
  int fd = open(fp, O_RDONLY);

  if (fd < 1) {
    RAISE_ERRNO(kIOError, "error opening file '%s'", fp);
  }

  std::unique_ptr<FileInputStream> stream(new FileInputStream(fd, true));
  stream->readNextChunk();
  stream->setFileName(file_path);
  return stream;
}

std::unique_ptr<FileInputStream> FileInputStream::fromFileDescriptor(
    int fd,
    bool close_on_destroy /* = false */) {
  std::unique_ptr<FileInputStream> stream(
      new FileInputStream(fd, close_on_destroy));

  stream->setFileName(StringUtil::format("<fd:$0>", fd));
  return stream;
}

std::unique_ptr<FileInputStream> FileInputStream::fromFile(File&& file) {
  auto fd = file.fd();

  std::unique_ptr<FileInputStream> stream(new FileInputStream(std::move(file)));
  stream->setFileName(StringUtil::format("<fd:$0>", fd));
  return stream;
}

FileInputStream::FileInputStream(
    int fd,
    bool close_on_destroy /* = false */) :
    fd_(fd),
    close_on_destroy_(close_on_destroy),
    buf_len_(0),
    buf_pos_(0) {}

FileInputStream::FileInputStream(
    File&& file) :
    FileInputStream(file.releaseFD(), true) {}

FileInputStream::~FileInputStream() {
  if (fd_ >= 0 && close_on_destroy_) {
    close(fd_);
  }
}

bool FileInputStream::readNextByte(char* target) {
  if (buf_pos_ >= buf_len_) {
    readNextChunk();
  }

  if (buf_pos_ < buf_len_) {
    *target = buf_[buf_pos_++];
    return true;
  } else {
    return false;
  }
}

size_t FileInputStream::skipNextBytes(size_t nbytes) {
  auto buf_remaining = buf_len_ - buf_pos_;

  if (nbytes <= buf_remaining) {
    buf_pos_ += nbytes;
    return nbytes;
  }

  buf_pos_ = 0;
  buf_len_ = 0;

  if (lseek(fd_, nbytes - buf_remaining, SEEK_CUR) == -1) {
    RAISE_ERRNO("seek() failed");
  }

  return nbytes;
}

bool FileInputStream::eof() {
  if (buf_pos_ >= buf_len_) {
    readNextChunk();
  }

  return buf_pos_ >= buf_len_;
}

// FIXPAUL move somwhere else...
FileInputStream::kByteOrderMark FileInputStream::readByteOrderMark() {
  if (buf_pos_ >= buf_len_) {
    readNextChunk();
  }

  static char kByteOrderMarkUTF8[] = { '\xEF', '\xBB', '\xBF' };
  if (buf_pos_ + sizeof(kByteOrderMarkUTF8) <= buf_len_ &&
      strncmp(
          buf_ + buf_pos_,
          kByteOrderMarkUTF8,
          sizeof(kByteOrderMarkUTF8)) == 0) {
    buf_pos_ += sizeof(kByteOrderMarkUTF8);
    return BOM_UTF8;
  }

  static char kByteOrderMarkUTF16[] = { '\xFF', '\xFE' };
  if (buf_pos_ + sizeof(kByteOrderMarkUTF16) <= buf_len_ &&
      strncmp(
          buf_ + buf_pos_,
          kByteOrderMarkUTF16,
          sizeof(kByteOrderMarkUTF16)) == 0) {
    buf_pos_ += sizeof(kByteOrderMarkUTF16);
    return BOM_UTF16;
  }

  return BOM_UNKNOWN;
}

void FileInputStream::readNextChunk() {
  int bytes_read = read(fd_, buf_, sizeof(buf_));

  if (bytes_read < 0) {
    RAISE_ERRNO(kIOError, "read(%s) failed", getFileName().c_str());
  }

  buf_pos_ = 0;
  buf_len_ = bytes_read;
}

void FileInputStream::rewind() {
  buf_pos_ = 0;
  buf_len_ = 0;

  if (lseek(fd_, 0, SEEK_SET) < 0) {
    RAISE_ERRNO(kIOError, "lseek(%s) failed", getFileName().c_str());
  }
}

} // namespace stx
