/*
 * This file is part of the "libfnord" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_FTS_GERMANSTEMMER_H
#define _FNORD_FTS_GERMANSTEMMER_H
#include "fnord-base/stdtypes.h"
#include "fnord-fts/Stemmer.h"
#include "fnord-fts/Hunspell.h"
#include "fnord-fts/SynonymDictionary.h"

namespace fnord {
namespace fts {

class GermanStemmer : public Stemmer {
public:

  GermanStemmer(
      const String& hunspell_aff_file,
      const String& hunspell_dict_file,
      const String& hunspell_hyphen_file,
      SynonymDictionary* synonyms);

  void stem(Language lang, String* term) override;

protected:

  void removeUmlauts(String* term);
  void stemWithUmlauts(Language lang, String* term);

  Hunspell hunspell_;
  SynonymDictionary* synonyms_;
};

} // namespace fts
} // namespace fnord

#endif
