/*
 * This file is part of the "libfnord" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-fts/StopwordDictionary.h"

namespace fnord {
namespace fts {

StopwordDictionary::StopwordDictionary() {}

bool StopwordDictionary::isStopword(Language lang, const String& term) const {
  return stopwords_.count(stopwordKey(lang, term)) != 0;
}

void StopwordDictionary::addStopword(Language lang, const String& term) {
  stopwords_.emplace(stopwordKey(lang, term));
}

} // namespace fts
} // namespace fnord
