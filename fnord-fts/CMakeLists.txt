project(FNORD_FTS)

find_package(Threads)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-static")

include_directories(../)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE FNORD_FTS_SOURCES
  Analyzer.cc
  SimpleTokenizer.cc
  GermanStemmer.cc
  Hunspell.cc
  StopwordDictionary.cc
  SynonymDictionary.cc
  ../3rdparty/hunspell/affentry.cxx
  ../3rdparty/hunspell/affixmgr.cxx
  ../3rdparty/hunspell/csutil.cxx
  ../3rdparty/hunspell/dictmgr.cxx
  ../3rdparty/hunspell/filemgr.cxx
  ../3rdparty/hunspell/hashmgr.cxx
  ../3rdparty/hunspell/hunspell.cxx
  ../3rdparty/hunspell/hunzip.cxx
  ../3rdparty/hunspell/phonet.cxx
  ../3rdparty/hunspell/replist.cxx
  ../3rdparty/hunspell/suggestmgr.cxx
  ../3rdparty/hunspell/utf_info.cxx
  ../3rdparty/hunspell/hnjalloc.c
  ../3rdparty/hunspell/hnjalloc.c
  ../3rdparty/hunspell/hyphen.c)

set(FNORD_FTS_LIBS
    ${CMAKE_THREAD_LIBS_INIT}
    z
    PARENT_SCOPE)

set(FNORD_FTS_INCLUDE
    PARENT_SCOPE)

add_library(fnord-fts OBJECT ${FNORD_FTS_SOURCES})
