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

#include "fnord-fts/analysis/Analyzer.h"

namespace fnord {
namespace fts {

class AnalyzerAdapter : public Analyzer {
public:

  TokenStreamPtr tokenStream(
      const String& field_name,
      const ReaderPtr& reader) override;

};


class TokenStreamAdapter : public TokenStream {
public:

    bool incrementToken() override;

};

}
}
#endif
