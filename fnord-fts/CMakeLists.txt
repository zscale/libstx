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
find_package(hunspell)

file(GLOB_RECURSE FNORD_FTS_SOURCES
  QueryAnalyzer.cc
  GermanStemmer.cc
  Hunspell.cc
  StopwordDictionary.cc
  SynonymDictionary.cc)

  #search/*.cc
  #analysis/*.cc
  #document/*.cc
  #index/*.cc
  #queryparser/*.cc
  #store/*.cc
  #util/*.c*)

set(FNORD_FTS_LIBS
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    ${HUNSPELL_LIBRARIES}
    z
    PARENT_SCOPE)

set(FNORD_FTS_INCLUDE
    ${Boost_INCLUDE_DIR}
    PARENT_SCOPE)

add_library(fnord-fts OBJECT ${FNORD_FTS_SOURCES})
