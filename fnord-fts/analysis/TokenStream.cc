/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

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
    term_att_->setTermLength(term.length());
    term_att_->resizeTermBuffer(term.length() + 1);
    auto trgt = term_att_->termBuffer();
    memcpy(trgt.get(), term.data(), trgt.size());
    return true;
  }
}

void TokenStream::addToken(const fnord::String token) {
  terms_.emplace_back(token);
}

}
}
