/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _STX_JSON_DOCUMENT_H
#define _STX_JSON_DOCUMENT_H
#include <functional>
#include <stdlib.h>
#include <string>
#include <vector>
#include "stx/json/jsoninputstream.h"
#include "stx/json/jsonpointer.h"

namespace stx {
namespace json {

class JSONDocument {
public:

  explicit JSONDocument(std::string json);
  explicit JSONDocument(std::unique_ptr<InputStream> json);
  explicit JSONDocument(std::unique_ptr<JSONInputStream> json);
  explicit JSONDocument(JSONInputStream&& json);

  JSONDocument(const JSONDocument& other) = delete;
  JSONDocument& operator=(const JSONDocument& other) = delete;

  bool getMaybe(const JSONPointer& path, std::string* dst) const;
  std::string get(const JSONPointer& path) const;
  std::string get(const JSONPointer& path, const std::string& fallback) const;

  template <typename T>
  bool getMaybeAs(const JSONPointer& path, T* dst) const;

  template <typename T>
  T getAs(const JSONPointer& path) const;

  template <typename T>
  T getAs(const JSONPointer& path, const T& fallback) const;

  void forEach(
      const JSONPointer& path,
      std::function<bool (const JSONPointer& path)> callback) const;

protected:
  std::vector<std::pair<JSONPointer, std::string>> data_; // FIXPAUL!!!
};

} // namespace json
} // namespace stx

#include "jsondocument_impl.h"
#endif
