// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include "Flower.h"
#include <xzero-flow/AST.h>
#include <xzero-flow/ASTPrinter.h>
#include <xzero-flow/FlowParser.h>
#include <xzero-flow/IRGenerator.h>
#include <xzero-flow/TargetCodeGenerator.h>
#include <xzero-flow/FlowCallVisitor.h>
#include <xzero-flow/ir/IRProgram.h>
#include <xzero-flow/ir/IRHandler.h>
#include <xzero-flow/ir/BasicBlock.h>
#include <xzero-flow/ir/Instr.h>
#include <xzero-flow/ir/ConstantArray.h>
#include <xzero-flow/ir/ConstantValue.h>
#include <xzero-flow/ir/PassManager.h>
#include <xzero-flow/transform/EmptyBlockElimination.h>
#include <xzero-flow/transform/InstructionElimination.h>
#include <xzero-flow/transform/UnusedBlockPass.h>
#include <xzero-flow/vm/Runtime.h>
#include <xzero-flow/vm/NativeCallback.h>
#include <xzero-flow/vm/Runner.h>
#include <fstream>
#include <memory>
#include <utility>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <iostream>
#include <inttypes.h>

using namespace xzero;

/*
 * TODO
 * - After parsing, traverse throught the AST and translate every assert()
 *   to also pass the string representation of the first argument as last
 *   argument.
 *
 */

void reportError(const char* category, const std::string& msg) {
  printf("%s error: %s\n", category, msg.c_str());
}

Flower::Flower()
    : Runtime(),
      filename_(),
      program_(nullptr),
      totalCases_(0),
      totalSuccess_(0),
      totalFailed_(0),
      dumpAST_(false),
      dumpIR_(false),
      dumpTarget_(false) {

  // properties
  registerFunction("cwd", flow::FlowType::String)
      .bind(&Flower::flow_getcwd);

  // functions
  registerFunction("random", flow::FlowType::Number)
      .bind(&Flower::flow_random);

  registerFunction("__print", flow::FlowType::Void)
      .params(flow::FlowType::String)
      .bind(&Flower::flow_print);

  registerFunction("__print", flow::FlowType::Void)
      .params(flow::FlowType::Number)
      .bind(&Flower::flow_print_I);

  registerFunction("__print", flow::FlowType::Void)
      .params(flow::FlowType::String, flow::FlowType::Number)
      .bind(&Flower::flow_print_SI);

  registerFunction("__print", flow::FlowType::Void)
      .params(flow::FlowType::Number, flow::FlowType::String)
      .bind(&Flower::flow_print_IS);

  registerFunction("__print", flow::FlowType::Void)
      .params(flow::FlowType::IntArray)
      .bind(&Flower::flow_print_i);

  registerFunction("__print", flow::FlowType::Void)
      .params(flow::FlowType::StringArray)
      .bind(&Flower::flow_print_s);

  registerFunction("__print", flow::FlowType::Void)
      .params(flow::FlowType::IPAddrArray)
      .bind(&Flower::flow_print_p);

  registerFunction("__print", flow::FlowType::Void)
      .params(flow::FlowType::CidrArray)
      .bind(&Flower::flow_print_c);

  registerFunction("suspend", flow::FlowType::Void)
      .bind(&Flower::flow_suspend);

  registerFunction("log", flow::FlowType::Void)
      .param<flow::FlowString>("message", "<whaaaaat!>")
      .param<flow::FlowNumber>("severity", 42)
      .bind(&Flower::flow_log);

  // unit test aiding handlers
  registerHandler("error")
      .param<flow::FlowString>("message", "")
      .bind(&Flower::flow_error);

  registerHandler("finish")
      .bind(&Flower::flow_finish);  // XXX rename to 'success'

  registerHandler("assert")
      .param<bool>("condition")
      .param<flow::FlowString>("description", "")
      .bind(&Flower::flow_assert);

  registerHandler("assert_fail")
      .param<bool>("condition")
      .param<flow::FlowString>("description", "")
      .bind(&Flower::flow_assertFail);

  registerHandler("fail")
      .param<flow::FlowNumber>("a1", 0)
      .param<flow::FlowNumber>("a2", 0)
      .bind(&Flower::flow_fail);

  registerHandler("pass")
      .param<flow::FlowNumber>("a1", 0)
      .param<flow::FlowNumber>("a2", 0)
      .bind(&Flower::flow_pass);

  registerFunction("numbers", flow::FlowType::Void)
      .param<flow::FlowIntArray>("values")
      .verifier(&Flower::verify_numbers)
      .bind(&Flower::flow_numbers);

  registerFunction("names", flow::FlowType::Void)
      .param<flow::FlowStringArray>("values")
      .bind(&Flower::flow_names);
}

Flower::~Flower() {
}

bool Flower::import(const std::string& name, const std::string& path,
                    std::vector<flow::vm::NativeCallback*>* builtins) {
  return false;
}

bool Flower::onParseComplete(flow::Unit* unit) {
  flow::FlowCallVisitor callv(unit);

  for (auto& call : callv.calls()) {
    if (call->callee()->name() != "assert" &&
        call->callee()->name() != "assert_fail")
      continue;

    flow::ParamList& args = call->args();
    assert(args.size() == 2);

    // add a string argument that equals the expression's source code
    flow::Expr* arg = args.values()[0];

    if (flow::StringExpr* descriptionExpr =
            dynamic_cast<flow::StringExpr*>(args.values()[1])) {
      std::string description = descriptionExpr->value();
      if (!description.empty()) {
        // XXX Preserve custom passed value.
        continue;
      }
    }

    std::string source = arg->location().text();
    args.replace(
        1,
        std::make_unique<flow::StringExpr>(source, flow::FlowLocation()));
  }

  return true;
}

int Flower::runAll(const char* fileName) {
  filename_ = fileName;

  flow::FlowParser parser(this);

  parser.importHandler = [&](const std::string& name,
                             const std::string& basedir,
                             std::vector<flow::vm::NativeCallback*>*)
                             ->bool {
    fprintf(stderr, "parser.importHandler('%s', '%s')\n", name.c_str(),
            basedir.c_str());
    return false;
  };

  if (!parser.open(fileName)) {
    fprintf(stderr, "Failed to open file: %s\n", fileName);
    return -1;
  }

  std::unique_ptr<flow::Unit> unit = parser.parse();
  if (!unit) {
    fprintf(stderr, "Failed to parse file: %s\n", fileName);
    return -1;
  }

  onParseComplete(unit.get());

  if (dumpAST_)
    flow::ASTPrinter::print(unit.get());

  if (!compile(unit.get())) return -1;

  for (auto handler : program_->handlers()) {
    if (strncmp(handler->name().c_str(), "test_", 5) != 0) continue;

    printf("[ -------- ] Testing %s\n", handler->name().c_str());
    totalCases_++;

    bool failed = handler->run(nullptr);
    if (failed) totalFailed_++;

    printf("[ -------- ] %s\n\n", failed ? "FAILED" : "OK");
  }

  printf("[ ======== ] %zu tests from %zu cases ran\n",
         totalSuccess_ + totalFailed_, totalCases_);
  if (totalSuccess_) printf("[  PASSED  ] %zu tests\n", totalSuccess_);
  if (totalFailed_) printf("[  FAILED  ] %zu tests\n", totalFailed_);

  return totalFailed_;
}

void printDefUseChain(flow::IRProgram* program) {
  printf("================================================ def-use chain\n");
  for (flow::IRHandler* handler : program->handlers()) {
    printf("handler:\n");
    for (flow::BasicBlock* bb : handler->basicBlocks()) {
      printf("bb:\n");
      for (flow::Instr* instr : bb->instructions()) {
        printf("def : ");
        instr->dump();
        for (flow::Instr* use : instr->uses()) {
          printf("use : ");
          use->dump();
        }
        if (instr->uses().empty()) {
          printf("no uses\n");
        }
        printf("\n");
      }
      printf("\n");
    }
    printf("\n");
  }
}

bool Flower::compile(flow::Unit* unit) {
  std::unique_ptr<flow::IRProgram> ir = flow::IRGenerator::generate(unit, {});
  if (!ir) {
    fprintf(stderr, "IR generation failed. Aborting.\n");
    return false;
  }

  {
    flow::PassManager pm;
    pm.registerPass(std::make_unique<flow::UnusedBlockPass>());

    // optional optimization passes
    if (1) {
      pm.registerPass(std::make_unique<flow::EmptyBlockElimination>());
      pm.registerPass(std::make_unique<flow::InstructionElimination>());
    }
    pm.run(ir.get());
  }

  if (dumpIR_) {
    ir->dump();
  }

  if (!verify(ir.get())) {
    fprintf(stderr, "User verification failed.\n");
    return false;
  }

  program_ = flow::TargetCodeGenerator().generate(ir.get());

  if (!program_) {
    fprintf(stderr, "Code generation failed. Aborting.\n");
    return false;
  }

  if (!program_->link(this)) {
    fprintf(stderr, "Program linking failed. Aborting.\n");
    return -1;
  }

  if (dumpTarget_) {
    program_->dump();
  }

  return true;
}

int Flower::run(const char* fileName, const char* handlerName) {
  if (!handlerName || !*handlerName) {
    printf("No handler specified.\n");
    return -1;
  }

  filename_ = fileName;

  flow::FlowParser parser(this);

  parser.importHandler = [&](const std::string& name,
                             const std::string& basedir,
                             std::vector<flow::vm::NativeCallback*>*)
                             -> bool {
    fprintf(stderr, "parser.importHandler('%s', '%s')\n", name.c_str(),
            basedir.c_str());
    return false;
  };

  if (!parser.open(fileName)) {
    fprintf(stderr, "Failed to open file: %s\n", fileName);
    return -1;
  }

  std::unique_ptr<flow::Unit> unit = parser.parse();
  if (!unit) {
    fprintf(stderr, "Failed to parse file: %s\n", fileName);
    return -1;
  }

  onParseComplete(unit.get());

  if (dumpAST_)
    flow::ASTPrinter::print(unit.get());

  flow::Handler* handlerSym = unit->findHandler(handlerName);
  if (!handlerSym) {
    fprintf(stderr, "No handler with name '%s' found in unit '%s'.\n",
            handlerName, fileName);
    return -1;
  }

  if (!compile(unit.get())) return -1;

  flow::vm::Handler* handler = program_->findHandler(handlerName);
  assert(handler != nullptr);

  printf("Running handler %s ...\n", handlerName);
  auto vm = handler->createRunner();
  bool rv = vm->run();
  while (vm->isSuspended()) {
    printf("Handler was suspended. Resuming.\n");
    rv = vm->resume();
  }
  return rv;
}

void Flower::dump() { program_->dump(); }

void Flower::flow_print(flow::vm::Params& args) {
  printf("%s\n", args.getString(1).str().c_str());
}

void Flower::flow_print_I(flow::vm::Params& args) {
  printf("%" PRIi64 "\n", args.getInt(1));
}

void Flower::flow_print_SI(flow::vm::Params& args) {
  printf("%s %" PRIi64 "\n", args.getString(1).str().c_str(), args.getInt(2));
}

void Flower::flow_print_IS(flow::vm::Params& args) {
  printf("%" PRIi64 " %s\n", args.getInt(1), args.getString(2).str().c_str());
}

void Flower::flow_print_i(flow::vm::Params& args) {
  const flow::FlowIntArray& array = args.getIntArray(1);
  printf("int array size: #%zu\n", array.size());
  for (flow::FlowNumber number : array) {
    printf("%" PRIi64 "\n", number);
  }
  printf("\n");
}

void Flower::flow_print_s(flow::vm::Params& args) {
  const flow::FlowStringArray& array = args.getStringArray(1);
  std::cout << "string array: (" << array.size() << ") [";
  size_t i = 0;
  for (const flow::FlowString& value : array) {
    if (i++) std::cout << ", ";
    std::cout << '"' << value << '"';
  }
  std::cout << ']' << std::endl;
}

void Flower::flow_print_p(flow::vm::Params& args) {
  for (const IPAddress& ipaddr : args.getIPAddressArray(1)) {
    printf("%s\n", ipaddr.c_str());
  }
  printf("\n");
}

void Flower::flow_print_c(flow::vm::Params& args) {
  for (const Cidr& cidr : args.getCidrArray(1)) {
    printf("%s\n", cidr.str().c_str());
  }
  printf("\n");
}

void Flower::flow_suspend(flow::vm::Params& args) { args.caller()->suspend(); }

void Flower::flow_log(flow::vm::Params& args) {
  const flow::FlowString& message = args.getString(1);
  flow::FlowNumber severity = args.getInt(2);

  printf("<%" PRIi64 "> %s\n", severity, message.str().c_str());
}

void Flower::flow_assert(flow::vm::Params& args) {
  const flow::FlowString& sourceValue = args.getString(2);

  if (!args.getBool(1)) {
    printf("[   FAILED ] %s\n", sourceValue.str().c_str());
    args.setResult(true);
  } else {
    printf("[       OK ] %s\n", sourceValue.str().c_str());
    ++totalSuccess_;
    args.setResult(false);
  }
}

void Flower::flow_getcwd(flow::vm::Params& args) {
  char buf[PATH_MAX];

  args.setResult(getcwd(buf, sizeof(buf)) ? buf : strerror(errno));
}

void Flower::flow_random(flow::vm::Params& args) {
  srand(time(NULL));

  args.setResult(static_cast<flow::FlowNumber>(random()));
}

void Flower::flow_getenv(flow::vm::Params& args) {
  args.setResult(getenv(args.getString(1).str().c_str()));
}

void Flower::flow_error(flow::vm::Params& args) {
  if (args.size() == 2)
    printf("Error. %s\n", args.getString(1).str().c_str());
  else
    printf("Error\n");

  args.setResult(true);
}

void Flower::flow_finish(flow::vm::Params& args) {
  args.setResult(true);
}

void Flower::flow_fail(flow::vm::Params& args) {
  args.setResult(true);
}

void Flower::flow_pass(flow::vm::Params& args) {
  args.setResult(false);
}

void Flower::flow_assertFail(flow::vm::Params& args) {
  if (args.getBool(1)) {
    fprintf(stderr, "Assertion failed. %s\n", args.getString(2).str().c_str());
    args.setResult(true);
  } else {
    args.setResult(false);
  }
}

bool Flower::verify_numbers(flow::Instr* call) {
  printf("Verify numbers!\n");
  flow::ConstantArray* array = static_cast<flow::ConstantArray*>(call->operand(1));
  assert(array != nullptr);

  for (size_t i = 0, e = array->get().size(); i != e; ++i) {
    flow::ConstantInt* arg = static_cast<flow::ConstantInt*>(array->get()[i]);
    assert(arg != nullptr);
    if (arg->get() % 2) {
      printf("Odd numbers not allowed.\n");
      return false;
    }
  }
  return true;
}

void Flower::flow_numbers(flow::vm::Params& args) {
  const flow::FlowIntArray& array = args.getIntArray(1);

  for (flow::FlowNumber value : array) {
    printf("number: %" PRIi64 "\n", value);
  }
}

void Flower::flow_names(flow::vm::Params& args) {
  const flow::FlowStringArray& array = args.getStringArray(1);

  for (const flow::FlowString& value : array) {
    printf("string: %s\n", value.str().c_str());
  }
}
