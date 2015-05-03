/**
 * This file is part of the "libfnord" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_TSDB_TSDBNODE_H
#define _FNORD_TSDB_TSDBNODE_H
#include <fnord-base/stdtypes.h>
#include <fnord-base/random.h>
#include <fnord-base/option.h>
#include <fnord-base/thread/queue.h>
#include <fnord-mdb/MDB.h>
#include <fnord-tsdb/StreamProperties.h>
#include <fnord-tsdb/StreamChunk.h>
#include <fnord-tsdb/TSDBNodeRef.h>
#include <fnord-tsdb/CompactionWorker.h>

namespace fnord {
namespace tsdb {

class TSDBNode {
public:
  TSDBNode(
      const String& nodeid,
      const String& db_path);

  void configurePrefix(
      const String& stream_key_prefix,
      StreamProperties props);

  void insertRecord(
      const String& stream_key,
      uint64_t record_id,
      const Buffer& record,
      DateTime time);

  void start();
  void stop();

protected:

  RefPtr<StreamProperties> configFor(const String& stream_key) const;

  String nodeid_;
  TSDBNodeRef noderef_;
  Vector<Pair<String, RefPtr<StreamProperties>>> configs_;
  std::mutex mutex_;
  HashMap<String, RefPtr<StreamChunk>> chunks_;
  Vector<RefPtr<CompactionWorker>> compaction_workers_;
};

} // namespace tdsb
} // namespace fnord

#endif
