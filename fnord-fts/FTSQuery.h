/*
 * This file is part of the "libfnord" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_FTS_FTSQUERY_H
#define _FNORD_FTS_FTSQUERY_H
#include "fnord-fts/fts.h"
#include "fnord-fts/fts_common.h"
#include "fnord-fts/Analyzer.h"
#include "fnord-fts/search/DisjunctionMaxQuery.h"

namespace fnord {
namespace fts {

class FTSQuery {
public:

  FTSQuery();

  void addField(const fnord::String& field_name, double boost = 1.0);

  void addTerm(const fnord::String& term);

  void addQuery(
      const fnord::String& query,
      fnord::Language lang,
      Analyzer* analyzer);

  void execute(IndexSearcher* searcher);

protected:
  struct FieldInfo {
    fnord::String field_name;
    double boost;
  };

  Vector<FieldInfo> fields_;
  fnord::Set<fnord::String> terms_;
};

}
}
#endif
