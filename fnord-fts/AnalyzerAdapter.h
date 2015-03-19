/**
 * Copyright (c) 2015 - The CM Authors <legal@clickmatcher.com>
 *   All Rights Reserved.
 *
 * This file is CONFIDENTIAL -- Distribution or duplication of this material or
 * the information contained herein is strictly forbidden unless prior written
 * permission is obtained.
 */
#ifndef _FNORD_FTS_ANALZERADAPTER_H
#define _FNORD_FTS_ANALZERADAPTER_H

#include "fnord-fts/analysis/Analyzer.h"

namespace fnord {
namespace fts {

class AnalyzerAdapter : public Analyzer {
public:

  TokenStreamPtr tokenStream(
      const String& fieldName,
      const ReaderPtr& reader) override;

};

}

}
#endif
