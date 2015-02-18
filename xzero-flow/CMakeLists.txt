cmake_minimum_required(VERSION 2.8)

project(xzero-flow)
set(XZERO_FLOW_VERSION "0.11.0-dev")

if(APPLE)
  set(CMAKE_CXX_FLAGS "-std=c++14 -stdlib=libc++")
else()
  set(CMAKE_CXX_FLAGS "-std=c++11")
endif()

set(xzero_flow_SRC
  AST.cpp
  ASTPrinter.cpp
  CMakeLists.txt
  FlowCallVisitor.cpp
  FlowLexer.cpp
  FlowLocation.cpp
  FlowParser.cpp
  FlowToken.cpp
  FlowType.cpp
  IRGenerator.cpp
  TargetCodeGenerator.cpp

  ir/BasicBlock.cpp
  ir/Constant.cpp
  ir/ConstantArray.cpp
  ir/ConstantValue.cpp
  ir/Instr.cpp
  ir/Instructions.cpp
  ir/InstructionVisitor.cpp
  ir/IRBuilder.cpp
  ir/IRHandler.cpp
  ir/IRProgram.cpp
  ir/PassManager.cpp
  ir/Value.cpp

  transform/EmptyBlockElimination.cpp
  transform/InstructionElimination.cpp
  transform/UnusedBlockPass.cpp

  vm/ConstantPool.cpp
  vm/Handler.cpp
  vm/Instruction.cpp
  vm/Match.cpp
  vm/MatchClass.cpp
  vm/NativeCallback.cpp
  vm/Program.cpp
  vm/Runner.cpp
  vm/Runtime.cpp
  vm/Signature.cpp
)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/sysconfig.h.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/sysconfig.h)

# libxzero-flow.so
add_library(xzero-flow SHARED ${xzero_flow_SRC})
target_link_libraries(xzero-flow pthread dl xzero-base)
set_target_properties(xzero-flow PROPERTIES VERSION ${XZERO_FLOW_VERSION})
set_target_properties(xzero-flow PROPERTIES COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden -DBUILD_XZERO=1")
install(TARGETS xzero-flow DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

# libxzero_flow.a
add_library(xzero_flow STATIC ${xzero_flow_SRC})
target_link_libraries(xzero-flow pthread dl xzero_base)
install(TARGETS xzero_flow DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

