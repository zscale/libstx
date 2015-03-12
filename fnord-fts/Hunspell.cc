/*
 * This file is part of the "libfnord" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-fts/Hunspell.h"

namespace fnord {
namespace fts {

Hunspell::Hunspell(const String& aff_file, const String& dict_file) {
  handle_ = Hunspell_create(aff_file.c_str(), dict_file.c_str());
}

} // namespace fts
} // namespace fnord

