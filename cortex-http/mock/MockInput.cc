// This file is part of the "x0" project, http://cortex.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <cortex-http/mock/MockInput.h>

namespace cortex {

MockInput::MockInput()
    : buffer_() {
}

int MockInput::read(Buffer* result) {
  size_t n = buffer_.size();
  result->push_back(buffer_);
  buffer_.clear();
  return n;
}

size_t MockInput::readLine(Buffer* result) {
  return 0; // TODO
}

void MockInput::onContent(const BufferRef& chunk) {
  buffer_ += chunk;
}

bool MockInput::empty() const noexcept {
  return buffer_.empty();
}

void MockInput::recycle() {
  buffer_.clear();
}

} // namespace cortex
