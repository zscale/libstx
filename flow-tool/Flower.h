// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero-flow/AST.h>
#include <xzero-flow/vm/Runtime.h>
#include <string>
#include <memory>
#include <cstdio>
#include <unistd.h>

namespace xzero {
  namespace flow {
    class Instr;
    namespace vm {
      class Program;
    } // vm
  } // flow
} // xzero

using namespace xzero;

class Flower : public flow::vm::Runtime {
 private:
  std::string filename_;
  std::unique_ptr<flow::vm::Program> program_;
  size_t totalCases_;    // total number of cases ran
  size_t totalSuccess_;  // total number of succeed tests
  size_t totalFailed_;   // total number of failed tests

  bool dumpAST_;
  bool dumpIR_;
  bool dumpTarget_;

 public:
  Flower();
  ~Flower();

  virtual bool import(const std::string& name, const std::string& path,
                      std::vector<flow::vm::NativeCallback*>* builtins);

  int optimizationLevel() { return 0; }
  void setOptimizationLevel(int val) {}

  void setDumpAST(bool value) { dumpAST_ = value; }
  void setDumpIR(bool value) { dumpIR_ = value; }
  void setDumpTarget(bool value) { dumpTarget_ = value; }

  int run(const char* filename, const char* handler);
  int runAll(const char* filename);
  void dump();

 private:
  bool onParseComplete(flow::Unit* unit);
  bool compile(flow::Unit* unit);

  // functions
  void flow_print(flow::vm::Params& args);
  void flow_print_I(flow::vm::Params& args);
  void flow_print_SI(flow::vm::Params& args);
  void flow_print_IS(flow::vm::Params& args);
  void flow_print_i(flow::vm::Params& args);
  void flow_print_s(flow::vm::Params& args);
  void flow_print_p(flow::vm::Params& args);
  void flow_print_c(flow::vm::Params& args);
  void flow_suspend(flow::vm::Params& args);
  void flow_log(flow::vm::Params& args);

  // handlers
  void flow_assert(flow::vm::Params& args);
  void flow_finish(flow::vm::Params& args);
  void flow_pass(flow::vm::Params& args);
  void flow_assertFail(flow::vm::Params& args);
  void flow_fail(flow::vm::Params& args);
  void flow_error(flow::vm::Params& args);
  void flow_getcwd(flow::vm::Params& args);
  void flow_random(flow::vm::Params& args);
  void flow_getenv(flow::vm::Params& args);

  bool verify_numbers(flow::Instr* call);
  void flow_numbers(flow::vm::Params& args);
  void flow_names(flow::vm::Params& args);

  // TODO: not ported yet
  //	void flow_mkbuf(flow::vm::Params& args);
  //	void flow_getbuf(flow::vm::Params& args);
};
