/*
 * This file is part of the "libfnord" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-fts/FTSQuery.h"

namespace fnord {
namespace fts {

FTSQuery::FTSQuery() {}

void FTSQuery::addField(const fnord::String& field_name, double boost) {
  FieldInfo fi;
  fi.field_name = field_name;
  fi.boost = boost;
  fields_.emplace_back(fi);
}

void FTSQuery::addTerm(const fnord::String& term) {
  terms_.emplace(term);
}

void FTSQuery::addQuery(
    const fnord::String& query,
    fnord::Language lang,
    Analyzer* analyzer) {
  analyzer->extractTerms(lang, query, [this] (const fnord::String& t) {
    addTerm(t);
  });
}

void FTSQuery::execute(IndexSearcher* searcher) {
  auto query = fts::newLucene<fts::DisjunctionMaxQuery>();

  auto t_query = fts::newLucene<fts::TermQuery>(
      fts::newLucene<fts::Term>(L"text~de", L"zubehoer"));

  query->add(t_query);

  auto collector = fts::TopScoreDocCollector::create(
      500,
      false);

  searcher->search(query, collector);
  fnord::iputs("return $0 docs", collector->getTotalHits());
}

}
}
