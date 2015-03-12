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
#include "fnord-fts/Hunspell.h"

namespace fnord {
namespace fts {

Hunspell::Hunspell(const String& aff_file, const String& dict_file) {
  handle_ = Hunspell_create(aff_file.c_str(), dict_file.c_str());
}

Hunspell::~Hunspell () {
  Hunspell_destroy(handle_);
}

Option<String> Hunspell::stem(const String& term) {
  char** res;
  Option<String> opt;

  auto cnt = Hunspell_stem(handle_, &res, term.c_str());
  if (cnt > 0) {
    opt = Some(String(res[cnt - 1]));
  }

  Hunspell_free_list(handle_, &res, cnt);
  return opt;
}

} // namespace fts
} // namespace fnord
