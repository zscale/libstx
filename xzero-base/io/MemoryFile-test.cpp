#include <gtest/gtest.h>
#include <xzero-base/io/MemoryFile.h>
#include <xzero-base/io/FileUtil.h>

using namespace xzero;

TEST(MemoryFile, simple) {
  const std::string path = "/foo.bar.txt";
  const std::string mimetype = "text/plain";
  const BufferRef data = "hello";
  const DateTime mtime(BufferRef("Thu, 12 Mar 2015 11:29:02 GMT"));

  MemoryFile file(path, mimetype, data, mtime);

  // printf("file.size: %zu\n", file.size());
  // printf("file.inode: %d\n", file.inode());
  // printf("file.mtime: %s\n", file.lastModified().c_str());
  // printf("file.etag: %s\n", file.etag().c_str());
  // printf("file.data: %s\n", FileUtil::read(file).c_str());

  EXPECT_TRUE(file.isRegular());
  EXPECT_TRUE(mtime.unixtime() == file.mtime());
  EXPECT_EQ(5, file.size());
  EXPECT_EQ("hello", FileUtil::read(file));
}

