project(FNORD_FTS)

find_package(Threads)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-static")
set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
add_definitions(-DBOOST_THREAD_DONT_USE_CHRONO)

find_package(Boost COMPONENTS date_time filesystem iostreams regex system thread REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIR})
include_directories(../)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE FNORD_FTS_SOURCES
  Analyzer.cc
  SimpleTokenizer.cc
  GermanStemmer.cc
  Hunspell.cc
  StopwordDictionary.cc
  SynonymDictionary.cc
  FTSQuery.cc
  AnalyzerAdapter.cc
  analysis/TokenStream.cc
  analysis/TokenFilter.cc
  analysis/NumericTokenStream.cc
  analysis/tokenattributes/TermAttribute.cc
  analysis/tokenattributes/TypeAttribute.cc
  analysis/tokenattributes/OffsetAttribute.cc
  analysis/tokenattributes/PayloadAttribute.cc
  analysis/tokenattributes/PositionIncrementAttribute.cc
  analysis/CachingTokenFilter.cc
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
  ../3rdparty/hunspell/hyphen.c
  search/*.cc
  document/*.cc
  index/*.cc
  queryparser/*.cc
  store/*.cc
  util/*.c*)

set(FNORD_FTS_LIBS
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    z
    PARENT_SCOPE)

set(FNORD_FTS_INCLUDE
    ${Boost_INCLUDE_DIR}
    PARENT_SCOPE)

add_library(fnord-fts OBJECT ${FNORD_FTS_SOURCES})
