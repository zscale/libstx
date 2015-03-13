cmake_minimum_required(VERSION 2.8)
project(flow-tool)

include(XzeroCommon)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(flow-tool main.cc Flower.cc)
target_link_libraries(flow-tool xzero-flow xzero-base)
