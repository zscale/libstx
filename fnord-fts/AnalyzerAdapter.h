/*
 * This file is part of the "libfnord" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_FTS_ANALZERADAPTER_H
#define _FNORD_FTS_ANALZERADAPTER_H

#include "fnord-fts/analysis/TokenStream.h"
#include "fnord-fts/util/CloseableThreadLocal.h"

namespace fnord {
namespace fts {

class AnalyzerAdapter : public LuceneObject {
public:
  virtual ~AnalyzerAdapter();
  LUCENE_CLASS(AnalyzerAdapter);
  virtual TokenStreamPtr tokenStream(const String& fieldName, const ReaderPtr& reader);
  virtual TokenStreamPtr reusableTokenStream(const String& fieldName, const ReaderPtr& reader);
  virtual int32_t getPositionIncrementGap(const String& fieldName);
  virtual int32_t getOffsetGap(const FieldablePtr& field);
  virtual void close();
protected:
  CloseableThreadLocal<LuceneObject> tokenStreams;
  virtual LuceneObjectPtr getPreviousTokenStream();
  virtual void setPreviousTokenStream(const LuceneObjectPtr& stream);
};

class TokenStreamAdapter : public TokenStream {
public:

    bool incrementToken() override;

};

}
}
#endif
