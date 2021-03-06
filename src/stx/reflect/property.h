/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _STX_REFLECT_PROPERTY_H
#define _STX_REFLECT_PROPERTY_H
#include <vector>
#include <string>
#include <functional>
#include "stx/reflect/indexsequence.h"

namespace stx {
namespace reflect {

template <typename ClassType, typename TargetType>
class PropertyReader {
public:
  PropertyReader(TargetType* target);

  template <typename PropertyType>
  void prop(
      PropertyType prop,
      uint32_t id,
      const std::string& prop_name,
      bool optional);

  const ClassType& instance() const;

protected:
  ClassType instance_;
  TargetType* target_;
};

template <typename ClassType, typename TargetType>
class PropertyWriter {
public:
  PropertyWriter(const ClassType& instance, TargetType* target);

  template <typename PropertyType>
  void prop(
      PropertyType prop,
      uint32_t id,
      const std::string& prop_name,
      bool optional);

protected:
  const ClassType& instance_;
  TargetType* target_;
};

template <typename TargetType>
class PropertyProxy {
public:
  PropertyProxy(TargetType* target);

  template <typename PropertyType>
  void prop(
      PropertyType prop,
      uint32_t id,
      const std::string& prop_name,
      bool optional);

protected:
  TargetType* target_;
};

}
}

#include "property_impl.h"
#endif
