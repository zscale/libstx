// This file is part of the "x0" project
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <xzero-base/Api.h>
#include <unordered_map>
#include <string>

namespace xzero {

class XZERO_API MimeTypes {
 public:
  MimeTypes();
  MimeTypes(const std::string& path, const std::string& defaultMimeType);

  /** Loads the mimetype map from given local file at @p path. */
  void loadFromLocal(const std::string& path);

  /** Retrieves the default mimetype. */
  const std::string& defaultMimeType() const XZERO_NOEXCEPT;

  /** Sets the default mimetype to given @p value. */
  void setDefaultMimeType(const std::string& value);

  /** Retrieves a mimetype based on given file @p path name. */
  const std::string& getMimeType(const std::string& path);

  /** Retrieves the mimetype mappings (from file extension to mimetype). */
  const std::unordered_map<std::string, std::string>& mimetypes() const XZERO_NOEXCEPT;

 private:
  std::unordered_map<std::string, std::string> mimetypes_;
  std::string defaultMimeType_;
};

// {{{ inlines
inline const std::unordered_map<std::string, std::string>&
    MimeTypes::mimetypes() const XZERO_NOEXCEPT {
  return mimetypes_;
}

inline const std::string& MimeTypes::defaultMimeType() const XZERO_NOEXCEPT {
  return defaultMimeType_;
}

inline void MimeTypes::setDefaultMimeType(const std::string& value) {
  defaultMimeType_ = value;
}
// }}}

}  // namespace xzero
