/*
 * This file is part of the "libfnord" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-fts/GermanStemmer.h"

namespace fnord {
namespace fts {

GermanStemmer::GermanStemmer(
      const String& hunspell_aff_file,
      const String& hunspell_dict_file,
      SynonymDictionary* synonyms) :
      hunspell_(hunspell_aff_file, hunspell_dict_file),
      synonyms_(synonyms) {}

void GermanStemmer::stem(Language lang, String* term) {
  auto synonym =  synonyms_->lookup(lang, *term);
  if (!synonym.isEmpty()) {
    term->assign(synonym.get());
    return;
  }

  auto stem = hunspell_.stem(*term);
  if (!stem.isEmpty()) {
    term->assign(stem.get());
    return;
  }
}

} // namespace fts
} // namespace fnord
