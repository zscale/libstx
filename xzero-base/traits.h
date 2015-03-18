// This file is part of the "libxzero" project, http://xzero.io/
//   (c) 2009-2015 Christian Parpart <trapni@gmail.com>
//   (c) 2014-2015 Paul Asmuth
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

namespace xzero {

/* TypeIsReflected<T> */
template <typename T>
using TypeIsReflected = xzero::reflect::is_reflected<T>;

/* TypeIsVector<T> */
template <typename T, typename = void>
struct TypeIsVector {
  static const bool value = false;
};

template <typename T>
struct TypeIsVector<
    T,
    typename std::enable_if<
        std::is_same<
            T,
            std::vector<
                typename T::value_type,
                typename T::allocator_type>>::value>::type>  {
  static const bool value = true;
};

} // namespace xzero
