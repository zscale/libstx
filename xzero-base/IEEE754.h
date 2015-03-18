/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <xzero-base/Api.h>
#include <cstdint>

namespace xzero {

class XZERO_API IEEE754 {
 public:
  static uint64_t toBytes(double value);
  static double fromBytes(uint64_t bytes);
};

} // namespace xzero
