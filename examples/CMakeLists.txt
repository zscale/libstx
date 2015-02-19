include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

include(XzeroCommon)

add_subdirectory(xzero-base)
add_subdirectory(xzero-http)
add_subdirectory(xzero-flow)
