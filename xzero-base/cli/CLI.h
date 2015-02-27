// This file is part of the "x0" project
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <xzero-base/Api.h>
#include <xzero-base/RuntimeError.h>
#include <xzero-base/cli/FlagType.h>
#include <functional>
#include <list>
#include <vector>
#include <string>

namespace xzero {

class IPAddress;
class Flags;

class XZERO_API CLI {
 public:
  CLI();

  CLI& defineString(
      const std::string& longOpt,
      char shortOpt,
      bool required,
      const std::string& helpText,
      const std::string& defaultValue,
      std::function<bool(const std::string&)> validator = nullptr);

  CLI& defineNumber(
      const std::string& longOpt,
      char shortOpt,
      bool required,
      const std::string& helpText,
      long int defaultValue,
      std::function<bool(long int)> validator = nullptr);

  CLI& defineFloat(
      const std::string& longOpt,
      char shortOpt,
      bool required,
      const std::string& helpText,
      float defaultValue,
      std::function<bool(float)> validator = nullptr);

  CLI& defineIPAddress(
      const std::string& longOpt,
      char shortOpt,
      bool required,
      const std::string& helpText,
      const IPAddress& defaultValue,
      std::function<bool(const IPAddress&)> validator = nullptr);

  CLI& defineBool(
      const std::string& longOpt,
      char shortOpt,
      const std::string& helpText);

  CLI& enableParameters(
      const std::string& valuePlaceholder,
      const std::string& helpText);

 private:
  CLI& define(
      const std::string& longOpt,
      char shortOpt,
      bool required,
      FlagType type,
      const std::string& helpText,
      const std::string& valuePlaceholder,
      const std::string& defaultValue,
      std::function<bool(const std::string&)> validator);

  class ValidationError;

 public:
  void evaluate(const std::vector<std::string>& args, Flags* out) const;
  void evaluate(int argc, const char* argv[], Flags* out) const;
  Flags evaluate(const std::vector<std::string>& args) const;
  Flags evaluate(int argc, const char* argv[]) const;

  std::string helpText(size_t width = 78, size_t helpTextOffset = 30) const;

 private:
  struct FlagDef;
  std::list<FlagDef> flagDefs_;

  // non-option parameters
  bool parametersEnabled_;
  std::string parametersPlaceholder_;
  std::string parametersHelpText_;
};

class XZERO_API CLI::ValidationError : public RuntimeError {
 public:
  ValidationError(
      const std::string& what, const char* sourceFile, int sourceLine)
      : RuntimeError(what, sourceFile, sourceLine) {}
};

struct XZERO_API CLI::FlagDef {
  FlagType type;
  std::string longOption;
  char shortOption;
  bool required;
  std::string helpText;
  std::string valuePlaceholder;
  std::string defaultValue;
  std::function<bool(const std::string&)> validator;

  std::string makeHelpText(size_t width, size_t helpTextOffset) const;
};

}  // namespace xzero
