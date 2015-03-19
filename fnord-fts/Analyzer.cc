/*
 * This file is part of the "libfnord" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord-base/io/fileutil.h>
#include <fnord-fts/GermanStemmer.h>
#include <fnord-fts/StopwordDictionary.h>
#include <fnord-fts/SynonymDictionary.h>
#include <fnord-fts/Analyzer.h>

namespace fnord {
namespace fts {

Analyzer::Analyzer(const fnord::String& conf) :
    german_stemmer_(
        FileUtil::joinPaths(conf, "hunspell_de.aff"),
        FileUtil::joinPaths(conf, "hunspell_de.dic"),
        FileUtil::joinPaths(conf, "hunspell_de.hyphen"),
        &synonyms_) {
  stopwords_.loadStopwordFile(
      FileUtil::joinPaths(conf, "stopwords.txt"));
}

void Analyzer::extractTerms(
    Language lang,
    const fnord::String& query,
    Function<void (const fnord::String& term)> term_callback) {
  tokenizer_.tokenize(query, [this, lang, term_callback] (const fnord::String& t) {
    if (stopwords_.isStopword(lang, t)) {
      return;
    }

    fnord::String term(t);
    stem(lang, &term);

    term_callback(term);
  });
}

void Analyzer::extractTerms(
    Language lang,
    const fnord::String& query,
    Set<fnord::String>* terms) {
  extractTerms(lang, query, [terms] (const fnord::String& term) {
    terms->emplace(term);
  });
}

void Analyzer::stem(Language lang, fnord::String* term) {
  switch (lang) {

    case Language::DE:
      german_stemmer_.stem(lang, term);
      break;

    default:
      break;

  }
}

}
}
