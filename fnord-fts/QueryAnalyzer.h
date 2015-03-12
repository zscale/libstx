/*
 * This file is part of the "libfnord" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_FTS_QUERYANALYZER_H
#define _FNORD_FTS_QUERYANALYZER_H
#include "fnord-base/stdtypes.h"
#include "fnord-base/Language.h"
#include "fnord-fts/StopwordDictionary.h"

namespace fnord {
namespace fts {

class QueryAnalyzer {
public:

  QueryAnalyzer(StopwordDictionary* stopwords);

  String normalize(Language lang, const String& query);

  void analyze(
      Language lang,
      const String& query,
      Set<String>* terms);

  void analyze(
      Language lang,
      const String& query,
      Function<void (const String& term)> term_callback);

  void tokenize(
      const String& query,
      Function<void (const String& term)> term_callback) const;

  void stem(Language lang, String* term) const;

  bool isStopword(Language lang, const String& term) const;

protected:
  StopwordDictionary* stopwords_;
};

} // namespace fts
} // namespace fnord

#endif
