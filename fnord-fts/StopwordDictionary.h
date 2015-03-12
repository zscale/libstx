/*
 * This file is part of the "libfnord" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_FTS_STOPWORDDICTIONARY_H
#define _FNORD_FTS_STOPWORDDICTIONARY_H
#include "fnord-base/stdtypes.h"
#include "fnord-base/Language.h"

namespace fnord {
namespace fts {

class StopwordDictionary {
public:

  StopwordDictionary();

  bool isStopword(Language lang, const String& term) const;

  void addStopword(Language lang, const String& term);
  void loadStopwordFile(const String& filename);

protected:

  inline String stopwordKey(Language lang, const String& term) const {
    String sw;
    sw.append((char *) &lang, sizeof(lang));
    sw += "~";
    sw += term;
    return sw;
  }

  Set<String> stopwords_;
};

} // namespace fts
} // namespace fnord

#endif
