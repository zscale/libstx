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
      const String& hunspell_dict_file) :
      hunspell_(hunspell_aff_file, hunspell_dict_file) {}

void GermanStemmer::stem(Language lang, String* term) {
  // synonym lookup

  auto stem = hunspell_.stem(*term);
  if (!stem.isEmpty()) {
    term->assign(stem.get());
  }
}

} // namespace fts
} // namespace fnord
