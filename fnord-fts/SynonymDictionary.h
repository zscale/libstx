/*
 * This file is part of the "libfnord" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_FTS_SYNONYMDICTIONARY_H
#define _FNORD_FTS_SYNONYMDICTIONARY_H
#include "fnord-base/stdtypes.h"
#include "fnord-base/option.h"
#include "fnord-base/Language.h"

namespace fnord {
namespace fts {

class SynonymDictionary {
public:

  Option<fnord::String> lookup(Language lang, const fnord::String& term);

  void addSynonym(
      Language lang,
      const fnord::String& term,
      const fnord::String& canonical_term);

  void loadSynonmFile(const fnord::String& filename);

protected:

  inline fnord::String synonymKey(Language lang, const fnord::String& term) const {
    fnord::String sw;
    sw += languageToString(lang);
    sw += "~";
    sw += term;
    return sw;
  }

  fnord::HashMap<fnord::String, fnord::String> synonyms_;
};

} // namespace fts
} // namespace fnord

#endif
