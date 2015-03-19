/*
 * This file is part of the "libfnord" project
 *   Copyright (c) 2015 Paul Asmuth
 *
* FnordMetric is free software: you can redistribute it and/or modify it under
* the terms of the GNU General Public License v3.0. You should have received a
* copy of the GNU General Public License along with this program. If not, see
* <http://www.gnu.org/licenses/>.
*/
#include "fnord-base/inspect.h"
#include "fnord-fts/fts.h"
#include "fnord-fts/fts_common.h"
#include "fnord-fts/AnalyzerAdapter.h"

namespace fnord {
namespace fts {

TokenStreamPtr AnalyzerAdapter::tokenStream(
    const String& field_name,
    const ReaderPtr& reader) {
  fnord::iputs("tokenize: $0", field_name);
  abort();
}

}
}
