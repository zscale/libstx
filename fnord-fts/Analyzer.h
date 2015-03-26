/*
 * This file is part of the "libfnord" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_FTS_ANALYZER_H
#define _FNORD_FTS_ANALYZER_H
#include <fnord-base/stdtypes.h>
#include <fnord-base/autoref.h>
#include "fnord-fts/SimpleTokenizer.h"
#include "fnord-fts/StopwordDictionary.h"
#include "fnord-fts/SynonymDictionary.h"
#include "fnord-fts/GermanStemmer.h"

namespace fnord {
namespace fts {

class Analyzer : public RefCounted {
public:

  Analyzer(const fnord::String& conf);

  void extractTerms(
      Language lang,
      const fnord::String& query,
      fnord::Set<fnord::String>* terms);

  void extractTerms(
      Language lang,
      const fnord::String& query,
      Function<void (const fnord::String& term)> term_callback);

  void stem(Language lang, fnord::String* term);

  fnord::String normalize(Language lang, const fnord::String& query);

protected:

  fnord::fts::SimpleTokenizer tokenizer_;
  fnord::fts::StopwordDictionary stopwords_;
  fnord::fts::SynonymDictionary synonyms_;
  fnord::fts::GermanStemmer german_stemmer_;
};

}
}
#endif
