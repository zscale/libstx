/*
 * This file is part of the "libfnord" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-base/UTF8.h"
#include "fnord-fts/QueryAnalyzer.h"

namespace fnord {
namespace fts {

QueryAnalyzer::QueryAnalyzer(
    StopwordDictionary* stopwords,
    Stemmer* stemmer) :
    stopwords_(stopwords),
    stemmer_(stemmer) {}

//String QueryAnalyzer::normalize(Language lang, const String& query) {}

void QueryAnalyzer::analyze(
    Language lang,
    const String& query,
    Set<String>* terms) {
  analyze(lang, query, [terms] (const String& term) {
    terms->emplace(term);
  });
}

void QueryAnalyzer::analyze(
    Language lang,
    const String& query,
    Function<void (const String& term)> term_callback) {
  tokenize(query, [this, lang, term_callback] (const String& t) {
    if (stopwords_->isStopword(lang, t)) {
      return;
    }

    String term(t);
    stemmer_->stem(lang, &term);

    term_callback(term);
  });
}

void QueryAnalyzer::tokenize(
    const String& query,
    Function<void (const String& term)> term_callback) const {
  String buf;

  auto cur = query.c_str();
  auto end = cur + query.size() + 1;
  while (cur < end) {
    auto chr = UTF8::nextCodepoint(&cur, end);

    // skip all symbols/dingbats characters
    if (chr > 0x2000 && chr < 0x2BFF) {
      continue;
    }

    switch (chr) {

      /* token boundaries */
      case '!':
      case '"':
      case '#':
      case '$':
      case '%':
      case '&':
      case '\'':
      case '(':
      case ')':
      case '*':
      case '+':
      case ',':
      case '.':
      case '-':
      case '/':
      case ':':
      case ';':
      case '<':
      case '=':
      case '>':
      case '?':
      case '@':
      case '[':
      case '\\':
      case ']':
      case '^':
      case '_':
      case '`':
      case '{':
      case '|':
      case '}':
      case '~':
      case ' ':
      case '\t':
      case '\r':
      case '\n':
      case 0:
        break;

      /* valid chars */
      default:
        UTF8::encodeCodepoint(std::tolower(chr), &buf);
        continue;

    }

    if (buf.length() > 0) {
      term_callback(buf);
      buf.clear();
    }
  }
}

} // namespace fts
} // namespace fnord

