/*
 * This file is part of the "libfnord" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-base/stringutil.h"
#include "fnord-fts/fts.h"
#include "fnord-fts/analysis/TokenStream.h"
#include "fnord-fts/analysis/tokenattributes/TermAttribute.h"
#include "fnord-fts/analysis/tokenattributes/OffsetAttribute.h"

namespace fnord {
namespace fts {

TokenStream::TokenStream() : pos_(0) {
  term_att_ = addAttribute<TermAttribute>();
  offset_att_ = addAttribute<OffsetAttribute>();
}

TokenStream::TokenStream(
    const AttributeSourcePtr& input) :
    AttributeSource(input),
    pos_(0) {}

TokenStream::TokenStream(
    const AttributeFactoryPtr& factory) :
    AttributeSource(factory),
    pos_(0) {}

TokenStream::~TokenStream() {}

void TokenStream::end() {}

void TokenStream::reset() {
  pos_ = 0;
}

void TokenStream::close() {}

bool TokenStream::incrementToken() {
  clearAttributes();

  if (++pos_ >= terms_.size()) {
    return false;
  } else {
    const auto& term = terms_[pos_];
    //offsetAtt->setOffset(correctOffset(0), finalOffset);

    // N.B. do not switch these two calls as the first one allocates memory for
    // the second. lucene magic... ;)
    term_att_->setTermBuffer(term);
    term_att_->setTermLength(term.length());
    return true;
  }
}

void TokenStream::addToken(const fnord::String token) {
  terms_.emplace_back(StringUtil::convertUTF8To16(token));
}

}
}
