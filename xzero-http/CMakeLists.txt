cmake_minimum_required(VERSION 2.8)

project(xzero-http)
set(XZERO_HTTP_VERSION "0.11.0-dev")

if(APPLE)
  set(CMAKE_CXX_FLAGS "-std=c++14 -stdlib=libc++")
else()
  set(CMAKE_CXX_FLAGS "-std=c++11")
endif()

set(xzero_http_SRC
  # common
  HeaderField.cpp
  HeaderFieldList.cpp
  HttpBufferedInput.cpp
  HttpChannel.cpp
  HttpConnectionFactory.cpp
  HttpDateGenerator.cpp
  HttpFile.cpp
  HttpFileHandler.cpp
  HttpInput.cpp
  HttpListener.cpp
  HttpMethod.cpp
  HttpOutput.cpp
  HttpOutputCompressor.cpp
  HttpRequest.cpp
  HttpResponse.cpp
  HttpService.cpp
  HttpStatus.cpp
  HttpTransport.cpp
  HttpVersion.cpp

  # http/1.1
  http1/Http1Channel.cpp
  http1/Http1ConnectionFactory.cpp
  http1/HttpConnection.cpp
  http1/HttpGenerator.cpp
  http1/HttpParser.cpp
)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/sysconfig.h.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/sysconfig.h)

# libxzero-http.so
add_library(xzero-http SHARED ${xzero_http_SRC})
target_link_libraries(xzero-http pthread dl xzero-base)
set_target_properties(xzero-http PROPERTIES VERSION ${XZERO_HTTP_VERSION})
set_target_properties(xzero-http PROPERTIES COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden -DBUILD_XZERO=1")
install(TARGETS xzero-http DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

# libxzero_http.a
add_library(xzero_http STATIC ${xzero_http_SRC})
target_link_libraries(xzero-http pthread dl xzero_base)
install(TARGETS xzero_http DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
