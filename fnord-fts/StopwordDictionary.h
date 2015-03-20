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

  bool isStopword(Language lang, const fnord::String& term) const;

  void addStopword(Language lang, const fnord::String& term);
  void loadStopwordFile(const fnord::String& filename);

protected:

  inline fnord::String stopwordKey(Language lang, const fnord::String& term) const {
    fnord::String sw;
    sw += languageToString(lang);
    sw += "~";
    sw += term;
    return sw;
  }

  fnord::Set<fnord::String> stopwords_;
};

} // namespace fts
} // namespace fnord

#endif
