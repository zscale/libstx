// This file is part of the "x0" project
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/MimeTypes.h>
#include <xzero-base/io/FileUtil.h>
#include <xzero-base/Tokenizer.h>
#include <xzero-base/Buffer.h>

namespace xzero {

MimeTypes::MimeTypes()
    : MimeTypes("/etc/mime.types", "application/octet-stream") {
}

MimeTypes::MimeTypes(const std::string& path,
                     const std::string& defaultMimeType)
    : mimetypes_(),
      defaultMimeType_(defaultMimeType) {

  if (!path.empty()) {
    loadFromLocal(path);
  }
}

/** Loads the mimetype map from given local file at @p path. */
void MimeTypes::loadFromLocal(const std::string& path) {
  Buffer input = FileUtil::read(path);

  mimetypes_.clear();
  auto lines = Tokenizer<BufferRef, Buffer>::tokenize(input, "\n");

  for (auto line : lines) {
    line = line.trim();
    auto columns = Tokenizer<BufferRef, BufferRef>::tokenize(line, " \t");

    auto ci = columns.begin(), ce = columns.end();
    BufferRef mime = ci != ce ? *ci++ : BufferRef();

    if (!mime.empty() && mime[0] != '#') {
      for (; ci != ce; ++ci) {
        mimetypes_[ci->str()] = mime.str();
      }
    }
  }
}

const std::string& MimeTypes::getMimeType(const std::string& path) {
  std::string* result = nullptr;

  // query mimetype
  const size_t ndot = path.find_last_of(".");
  const size_t nslash = path.find_last_of("/");

  if (ndot != std::string::npos && ndot > nslash) {
    std::string ext(path.substr(ndot + 1));

    while (ext.size()) {
      auto i = mimetypes_.find(ext);

      if (i != mimetypes_.end())
        result = &i->second;

      if (ext[ext.size() - 1] != '~')
        break;

      ext.resize(ext.size() - 1);
    }

    if (!result || result->empty()) {
      result = &defaultMimeType_;
    }
  } else {
    result = &defaultMimeType_;
  }

  return *result;
}

}  // namespace xzero
