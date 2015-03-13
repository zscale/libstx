cmake_minimum_required(VERSION 2.8)

project(xzero-base CXX)
set(XZERO_BASE_VERSION "0.11.0-dev")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)
include(XzeroCommon)
include(FindPkgConfig)
include(FindDoxygen)
include(FindOpenSSL)
include(CheckIncludeFiles)
include(CheckIncludeFileCXX)
include(CheckFunctionExists)
include(CheckVariableExists)
include(CheckTypeSize)
include(CheckLibraryExists)
include(CheckCSourceCompiles)
include(CMakeDetermineCCompiler)

CHECK_INCLUDE_FILES(sys/sendfile.h HAVE_SYS_SENDFILE_H)
CHECK_FUNCTION_EXISTS(sendfile HAVE_SENDFILE)
CHECK_FUNCTION_EXISTS(posix_fadvise HAVE_POSIX_FADVISE)
CHECK_FUNCTION_EXISTS(readahead HAVE_READAHEAD)
CHECK_FUNCTION_EXISTS(pread HAVE_PREAD)

CHECK_INCLUDE_FILES(unistd.h HAVE_UNISTD_H)
CHECK_INCLUDE_FILES(sys/inotify.h HAVE_SYS_INOTIFY_H)
CHECK_INCLUDE_FILES(sys/resource.h HAVE_SYS_RESOURCE_H)
CHECK_INCLUDE_FILES(sys/mman.h HAVE_SYS_MMAN_H)
CHECK_INCLUDE_FILES(sys/limits.h HAVE_SYS_LIMITS_H)
CHECK_INCLUDE_FILES(pwd.h HAVE_PWD_H)
CHECK_INCLUDE_FILES(syslog.h HAVE_SYSLOG_H)
CHECK_INCLUDE_FILES(dlfcn.h HAVE_DLFCN_H)
CHECK_INCLUDE_FILES(execinfo.h HAVE_EXECINFO_H)
CHECK_INCLUDE_FILES(uuid/uuid.h HAVE_UUID_UUID_H)

CHECK_FUNCTION_EXISTS(sysconf HAVE_SYSCONF)
CHECK_FUNCTION_EXISTS(pathconf HAVE_PATHCONF)
CHECK_FUNCTION_EXISTS(accept4 HAVE_ACCEPT4)
CHECK_FUNCTION_EXISTS(pipe2 HAVE_PIPE2)
CHECK_FUNCTION_EXISTS(dup2 HAVE_DUP2)
CHECK_FUNCTION_EXISTS(dladdr HAVE_DLADDR)
CHECK_FUNCTION_EXISTS(fork HAVE_FORK)
CHECK_FUNCTION_EXISTS(backtrace HAVE_BACKTRACE)
CHECK_FUNCTION_EXISTS(clock_gettime HAVE_CLOCK_GETTIME)

CHECK_INCLUDE_FILES(pthread.h HAVE_PTHREAD_H)
CHECK_LIBRARY_EXISTS(pthread pthread_setname_np "" HAVE_PTHREAD_SETNAME_NP)
CHECK_LIBRARY_EXISTS(pthread pthread_setaffinity_np "" HAVE_PTHREAD_SETAFFINITY_NP)

CHECK_INCLUDE_FILES(sys/inotify.h HAVE_SYS_INOTIFY_H)
if(HAVE_SYS_INOTIFY_H)
  CHECK_FUNCTION_EXISTS(inotify_init1 HAVE_INOTIFY_INIT1)
endif(HAVE_SYS_INOTIFY_H)

CHECK_INCLUDE_FILES(zlib.h HAVE_ZLIB_H)
if(HAVE_ZLIB_H)
  CHECK_LIBRARY_EXISTS(z deflate "" HAVE_LIBZ)
  set(ZLIB_LIBRARIES z)
endif(HAVE_ZLIB_H)

CHECK_INCLUDE_FILES(bzlib.h HAVE_BZLIB_H)
if(HAVE_BZLIB_H)
  CHECK_LIBRARY_EXISTS(bz2 BZ2_bzCompressInit "" HAVE_LIBBZ2)
  set(BZIP2_LIBRARIES bz2)
endif(HAVE_BZLIB_H)

# PCRE
option(ENABLE_PCRE "With PCRE support [default: off]" OFF)
if(ENABLE_PCRE)
  pkg_check_modules(PCRE REQUIRED libpcre>=7.0)
else()
  pkg_check_modules(PCRE libpcre>=7.0)
endif()

# required for shm_open/shm_unlink
if(APPLE)
  set(RT_LIBRARIES)
else()
  set(RT_LIBRARIES rt)
endif()

if(NOT OPENSSL_FOUND)
  message(FATAL_ERROR "OpenSSL not found ;-(")
else()
  message(STATUS "OpenSSL found ;-)")
endif()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/sysconfig.h.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/sysconfig.h)

set(xzero_base_SRC
  Base64.cpp
  Buffer.cpp
  DateTime.cpp
  IdleTimeout.cpp
  MimeTypes.cpp
  Random.cpp
  RegExp.cpp
  RuntimeError.cpp
  StackTrace.cpp
  StringUtil.cpp
  TimeSpan.cpp
  Tokenizer.cpp
  Uri.cpp
  WallClock.cpp

  cli/CLI.cpp
  cli/Flags.cpp

  logging/LogAggregator.cpp
  logging/LogLevel.cpp
  logging/LogSource.cpp
  logging/LogTarget.cpp
  logging.cpp

  executor/SafeCall.cpp
  executor/Executor.cpp
  executor/DirectExecutor.cpp
  executor/PosixScheduler.cpp
  executor/Scheduler.cpp
  executor/ThreadedExecutor.cpp
  executor/ThreadPool.cpp

  hash/FNV.cpp

  io/GzipFilter.cpp
  io/File.cpp
  io/FileDescriptor.cpp
  io/FileRef.cpp
  io/FileRepository.cpp
  io/Filter.cpp
  io/FileUtil.cpp
  io/LocalFile.cpp
  io/LocalFileRepository.cpp
  io/MemoryFile.cpp
  io/MemoryFileRepository.cpp
  io/MemoryMap.cpp

  net/ByteArrayEndPoint.cpp
  net/Cidr.cpp
  net/Connection.cpp
  net/ConnectionFactory.cpp
  net/Connector.cpp
  net/DnsClient.cpp
  net/EndPoint.cpp
  net/EndPointWriter.cpp
  net/InetConnector.cpp
  net/InetEndPoint.cpp
  net/LocalConnector.cpp
  net/Server.cpp
  net/SslConnector.cpp
  net/SslContext.cpp
  net/SslEndPoint.cpp
)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(XZERO_BASE_LIBRARIES pthread dl
    ${ZLIB_LIBRARIES} ${OPENSSL_LIBRARIES} ${PCRE_LIBRARIES} ${RT_LIBRARIES})

# libxzero-base.a
add_library(xzero-base STATIC ${xzero_base_SRC})
target_link_libraries(xzero-base ${XZERO_BASE_LIBRARIES})
#set_target_properties(xzero-base PROPERTIES COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden -DBUILD_XZERO_BASE=1")
#install(TARGETS xzero-base DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

# headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        FILES_MATCHING PATTERN "*.h"
                       PATTERN "CMakeFiles" EXCLUDE)

# test-base
file(GLOB_RECURSE xzero_base_test_SRC "*-test.cpp")
add_executable(test-base ../test-main.cpp ${xzero_base_test_SRC})
target_link_libraries(test-base xzero-base gtest)

# pkg-config target
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/xzero-base.pc.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/xzero-base.pc)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/xzero-base.pc 
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
