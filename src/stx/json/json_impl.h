/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _STX_JSON_IMPL_H
#define _STX_JSON_IMPL_H
#include "stx/exception.h"
#include "stx/inspect.h"
#include "stx/UnixTime.h"
#include "stx/traits.h"
#include "stx/json/jsonutil.h"
#include "stx/json/jsonoutputstream.h"
#include "stx/reflect/indexsequence.h"
#include "stx/reflect/reflect.h"

namespace stx {
namespace json {

template <typename T>
T fromJSONImpl(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end);

template <
    typename T,
    typename A = typename std::enable_if<TypeIsReflected<T>::value>::type>
T fromJSON(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end) {
  return JSONInputProxy<T>(begin, end).value;
}

template <
    typename T,
    typename A = typename std::enable_if<!TypeIsReflected<T>::value>::type,
    typename B = typename std::enable_if<TypeIsVector<T>::value>::type>
T fromJSON(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end) {
  if (begin == end) {
    RAISE(kIndexError);
  }

  if (begin->type != JSON_ARRAY_BEGIN) {
    RAISE(kIllegalArgumentError, "expected JSON_ARRAY_BEGIN");
  }

  std::vector<typename T::value_type> vec;
  for (begin++; begin != end; ) {
    if (begin->type == JSON_ARRAY_END) {
      break;
    }

    vec.emplace_back(fromJSON<typename T::value_type>(begin, end));
    begin += begin->size;
  }

  return vec;
}

template <
    typename T,
    typename A = typename std::enable_if<!TypeIsReflected<T>::value>::type,
    typename B = typename std::enable_if<!TypeIsVector<T>::value>::type,
    typename C = void>
T fromJSON(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end) {
  return fromJSONImpl<T>(begin, end);
}

template <typename T>
T fromJSON(const std::string& json_str) {
  return fromJSON<T>(parseJSON(json_str));
}

template <typename T>
T fromJSON(const stx::Buffer& json_buf) {
  return fromJSON<T>(parseJSON(json_buf));
}

template <typename T>
T fromJSON(const JSONObject& json_obj) {
  return fromJSON<T>(json_obj.begin(), json_obj.end());
}

template <
    typename T,
    typename O,
    typename = typename std::enable_if<
        stx::reflect::is_reflected<T>::value>::type>
void toJSON(const T& value, O* target) {
  JSONOutputProxy<O> proxy(value, target);
}

template <
    typename T,
    typename O,
    typename = typename std::enable_if<
        !stx::reflect::is_reflected<T>::value>::type,
    typename = void>
void toJSON(const T& value, O* target) {
  toJSONImpl(value, target);
}

template <typename T>
std::string toJSONString(const T& value) {
  std::string json_str;
  JSONOutputStream json(
      static_cast<std::unique_ptr<OutputStream>>(
          StringOutputStream::fromString(&json_str)));

  toJSON(value, &json);
  return json_str;
}

template <typename T>
JSONObject toJSON(const T& value) {
  JSONObject obj;
  toJSON(value, &obj);
  return obj;
}

template <typename T>
JSONInputProxy<T>::JSONInputProxy(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end) :
    obj_begin(begin),
    obj_end(end),
    value(stx::reflect::MetaClass<T>::unserialize(this)) {}

template <typename T>
template <typename PropertyType>
PropertyType JSONInputProxy<T>::getProperty(
    uint32_t id,
    const std::string& name) {
  auto iter = JSONUtil::objectLookup(obj_begin, obj_end, name);

  if (iter == obj_end) {
    RAISEF(kIndexError, "no such element: $0", name);
  }

  return fromJSON<PropertyType>(iter, obj_end);
}

template <typename T>
template <typename PropertyType>
Option<PropertyType> JSONInputProxy<T>::getOptionalProperty(
    uint32_t id,
    const std::string& name) {
  auto iter = JSONUtil::objectLookup(obj_begin, obj_end, name);

  if (iter == obj_end) {
    return None<PropertyType>();
  }

  return Some(fromJSON<PropertyType>(iter, obj_end));
}

template <typename OutputType>
template <typename T>
JSONOutputProxy<OutputType>::JSONOutputProxy(
    const T& instance,
    OutputType* target) :
    target_(target) {
  target_->emplace_back(json::JSON_OBJECT_BEGIN);
  stx::reflect::MetaClass<T>::serialize(instance, this);
  target_->emplace_back(json::JSON_OBJECT_END);
}

template <typename OutputType>
template <typename T>
void JSONOutputProxy<OutputType>::putProperty(
    uint32_t id,
    const std::string& name,
    const T& value) {
  target_->emplace_back(json::JSON_STRING, name);
  toJSON(value, target_);
}

template <typename T, typename O>
void toJSONImpl(const Vector<T>& value, O* target) {
  target->emplace_back(json::JSON_ARRAY_BEGIN);

  for (const auto& e : value) {
    toJSON(e, target);
  }

  target->emplace_back(json::JSON_ARRAY_END);
}

template <typename T, typename O>
void toJSONImpl(const Set<T>& value, O* target) {
  target->emplace_back(json::JSON_ARRAY_BEGIN);

  for (const auto& e : value) {
    toJSON(e, target);
  }

  target->emplace_back(json::JSON_ARRAY_END);
}

template <typename... T, typename H, typename O>
void toJSONVariadicImpl(O* target, const H& head, const T&... tail) {
  toJSON(head, target);
  toJSONVariadicImpl(target, tail...);
}

template <typename T, typename O>
void toJSONVariadicImpl(O* target, const T& head) {
  toJSON(head, target);
}

template <typename... T, int... I, typename O>
void toJSONTupleImpl(
    const std::tuple<T...>& value,
    O* target,
    stx::reflect::IndexSequence<I...>) {
  toJSONVariadicImpl(target, std::get<I>(value)...);
}

template <typename... T, typename O>
void toJSONImpl(const std::tuple<T...>& value, O* target) {
  target->emplace_back(json::JSON_ARRAY_BEGIN);
  toJSONTupleImpl(
      value,
      target,
      typename stx::reflect::MkIndexSequenceFor<T...>::type());
  target->emplace_back(json::JSON_ARRAY_END);
}

template <typename O>
void toJSONImpl(const std::string& str, O* target) {
  target->emplace_back(json::JSON_STRING, str);
}

template <typename O>
void toJSONImpl(const char* const& str, O* target) {
  target->emplace_back(json::JSON_STRING, str);
}

template <typename O>
void toJSONImpl(unsigned long long const& val, O* target) {
  target->emplace_back(json::JSON_NUMBER, StringUtil::toString(val));
}

template <typename O>
void toJSONImpl(unsigned long const& val, O* target) {
  target->emplace_back(json::JSON_NUMBER, StringUtil::toString(val));
}

template <typename O>
void toJSONImpl(int const& val, O* target) {
  target->emplace_back(json::JSON_NUMBER, StringUtil::toString(val));
}

template <typename O>
void toJSONImpl(unsigned int const& val, O* target) {
  target->emplace_back(json::JSON_NUMBER, StringUtil::toString(val));
}

template <typename O>
void toJSONImpl(double const& val, O* target) {
  target->emplace_back(json::JSON_NUMBER, StringUtil::toString(val));
}

template <typename O>
void toJSONImpl(const stx::UnixTime& val, O* target) {
  toJSONImpl(static_cast<uint64_t>(val), target);
}

template <typename O>
void toJSONImpl(const bool& val, O* target) {
  if (val) {
    target->emplace_back(json::JSON_TRUE);
  } else {
    target->emplace_back(json::JSON_FALSE);
  }
}

template <typename O>
void toJSONImpl(const JSONObject& obj, O* target) {
  for (const auto& token : obj) {
    target->emplace_back(token);
  }
}

template <typename T1, typename T2, typename O>
void toJSONImpl(const HashMap<T1, T2>& val, O* target) {
  target->emplace_back(json::JSON_OBJECT_BEGIN);

  for (const auto& pair : val) {
    toJSON(pair.first, target);
    toJSON(pair.second, target);
  }

  target->emplace_back(json::JSON_OBJECT_END);
}

template <typename T1, typename T2, typename O>
void toJSONImpl(const Pair<T1, T2>& val, O* target) {
  target->emplace_back(json::JSON_ARRAY_BEGIN);
  toJSON(val.first, target);
  toJSON(val.second, target);
  target->emplace_back(json::JSON_ARRAY_END);
}



}
}
#endif
