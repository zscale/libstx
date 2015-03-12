// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero-base/Api.h>
#include <xzero-base/Buffer.h>
#include <functional>
#include <iosfwd>
#include <string>
#include <memory>

namespace xzero {

class MemoryMap;

/**
 * HTTP servable file.
 *
 * @see FileHandler
 * @see LocalFile, MemoryFile
 * @see FileRepository
 */
class XZERO_API File {
 public:
  File(const File&) = delete;
  File& operator=(const File&) = delete;

  File(const std::string& path, const std::string& mimetype);

  virtual ~File();

  std::string filename() const;
  const std::string& path() const { return path_; }
  const std::string& mimetype() const { return mimetype_; }
  const std::string& lastModified() const;

  virtual const std::string& etag() const = 0;

  virtual size_t size() const XZERO_NOEXCEPT = 0;
  virtual time_t mtime() const XZERO_NOEXCEPT = 0;
  virtual size_t inode() const XZERO_NOEXCEPT = 0;
  virtual bool isRegular() const XZERO_NOEXCEPT = 0;

  /** Creates a file descriptor for this file. */
  virtual int tryCreateChannel() = 0;

  /** creates an input stream for given file. */
  virtual std::unique_ptr<std::istream> createInputChannel() = 0;

  /** creates an output stream for given file. */
  virtual std::unique_ptr<std::ostream> createOutputChannel() = 0;

  /** Creates a memory-map for a given file.
   *
   * @param rw weather to map file in read/write mode, or just in read-only.
   */
  virtual std::unique_ptr<MemoryMap> createMemoryMap(bool rw = true) = 0;

  /**
   * Sets file-error code, that is only used for the validity of the entity.
   */
  void setErrorCode(int ec) { errno_ = ec; }

  /**
   * Retrieves errno-compatible error code for the validity of the entity.
   */
  int errorCode() const XZERO_NOEXCEPT { return errno_; }

 private:
  std::string path_;
  std::string mimetype_;

  int errno_;

 protected:
  mutable std::string lastModified_;
};

} // namespace xzero
