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
    StopwordDictionary* stopwords) :
    stopwords_(stopwords) {}

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
    if (isStopword(lang, t)) {
      return;
    }

    String term(t);
    stem(lang, &term);

    term_callback(term);
  });
}

void QueryAnalyzer::stem(Language lang, String* term) const {
}

bool QueryAnalyzer::isStopword(Language lang, const String& term) const {
  return false;
}

void QueryAnalyzer::tokenize(
    const String& query,
    Function<void (const String& term)> term_callback) const {
  String buf;

  auto cur = query.c_str();
  auto end = cur + query.size();
  char32_t chr;
  while ((chr = UTF8::nextCodepoint(&cur, end)) != 0) {
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
        break;

      /* ignore chars */
      case L'✪':
        continue;

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

