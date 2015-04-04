cmake_minimum_required(VERSION 2.8)

project(xzero-http CXX)
set(XZERO_HTTP_VERSION "0.11.0-dev")

include(XzeroCommon)

set(xzero_http_SRC
  # common
  BadMessage.cc
  HeaderField.cc
  HeaderFieldList.cc
  HttpBufferedInput.cc
  HttpChannel.cc
  HttpConnectionFactory.cc
  HttpDateGenerator.cc
  HttpFileHandler.cc
  HttpInput.cc
  HttpListener.cc
  HttpMethod.cc
  HttpOutput.cc
  HttpOutputCompressor.cc
  HttpRequest.cc
  HttpResponse.cc
  HttpService.cc
  HttpStatus.cc
  HttpTransport.cc
  HttpVersion.cc

  # transport: mock
  mock/MockTransport.cc

  # transport: http/1.1
  http1/Http1Channel.cc
  http1/Http1ConnectionFactory.cc
  http1/HttpConnection.cc
  http1/HttpGenerator.cc
  http1/HttpParser.cc
)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/sysconfig.h.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/sysconfig.h)

set(XZERO_HTTP_LIBRARIES xzero-base)

# libxzero-http.a
add_library(xzero-http STATIC ${xzero_http_SRC})
#set_target_properties(xzero-http PROPERTIES COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden -DBUILD_XZERO_HTTP=1")
target_link_libraries(xzero-http ${XZERO_HTTP_LIBRARIES})
install(TARGETS xzero-http DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

# headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        FILES_MATCHING PATTERN "*.h"
                       PATTERN "CMakeFiles" EXCLUDE)

# test-http
file(GLOB_RECURSE xzero_http_test_SRC "*-test.cc")
add_executable(test-http ../test-main.cc ${xzero_http_test_SRC})
target_link_libraries(test-http xzero-http xzero-base gtest)

# pkg-config target
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/xzero-http.pc.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/xzero-http.pc)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/xzero-http.pc 
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
