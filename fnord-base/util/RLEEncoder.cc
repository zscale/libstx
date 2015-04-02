/**
 * This file is part of the "libfnord" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <3rdparty/simdcomp/simdcomp.h>
#include <fnord-base/inspect.h>
#include <fnord-base/util/RLEEncoder.h>

namespace fnord {
namespace util {

void RLEEncoder::encode(uint32_t value) {
  buf_.append(&value, sizeof(value));
}

void RLEEncoder::flush() {
}

void* RLEEncoder::data() const {
  return buf_.data();
}

size_t RLEEncoder::size() const {
  return buf_.size();
}

}
}
