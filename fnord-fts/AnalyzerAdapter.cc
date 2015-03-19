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
#include "fnord-base/UTF8.h"
#include "fnord-fts/fts.h"
#include "fnord-fts/fts_common.h"
#include "fnord-fts/AnalyzerAdapter.h"

namespace fnord {
namespace fts {

TokenStreamPtr AnalyzerAdapter::tokenStream(
    const String& field_name16,
    const ReaderPtr& reader) {
  std::string field_name = StringUtil::convertUTF16To8(field_name16);
  std::string field_str;
  wchar_t chr;
  while ((chr = reader->read()) > 0) {
    UTF8::encodeCodepoint(chr, &field_str);
  }

  fnord::iputs("tokenize: $0 => $1", field_name, field_str);
  return std::make_shared<TokenStreamAdapter>();
}

bool TokenStreamAdapter::incrementToken() {
  return false;
}

}
}
