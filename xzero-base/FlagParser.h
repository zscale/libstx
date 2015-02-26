// This file is part of the "x0" project
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <xzero-base/Api.h>
#include <xzero-base/RuntimeError.h>
#include <functional>
#include <list>
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>

namespace xzero {

class IPAddress;

enum class FlagType {
  String,
  Number,
  Float,
  IP,
  Bool,
};

typedef std::pair<FlagType, std::string> FlagValue;

class FlagValidationError : public RuntimeError {
 public:
  FlagValidationError(
      const std::string& what, const char* sourceFile, int sourceLine)
      : RuntimeError(what, sourceFile, sourceLine) {}
};

class XZERO_API Flags {
 public:
  Flags();

  std::string getString(const std::string& flag) const;
  long int getNumber(const std::string& flag) const;
  float getFloat(const std::string& flag) const;
  bool getBool(const std::string& flag) const;

  bool isSet(const std::string& flag) const;

 private:
  friend class FlagParser;

  std::unordered_map<std::string, FlagValue> set_;
};

class XZERO_API FlagParser {
 public:
  FlagParser& defineString(
      const std::string& longOpt,
      char shortOpt,
      bool required,
      const std::string& helpText,
      const std::string& defaultValue,
      std::function<bool(const std::string&)> validator = nullptr);

  FlagParser& defineNumber(
      const std::string& longOpt,
      char shortOpt,
      bool required,
      const std::string& helpText,
      long int defaultValue,
      std::function<bool(long int)> validator = nullptr);

  FlagParser& defineFloat(
      const std::string& longOpt,
      char shortOpt,
      bool required,
      const std::string& helpText,
      float defaultValue,
      std::function<bool(float)> validator = nullptr);

  FlagParser& defineIPAddress(
      const std::string& longOpt,
      char shortOpt,
      bool required,
      const std::string& helpText,
      const IPAddress& defaultValue,
      std::function<bool(const IPAddress&)> validator = nullptr);

  FlagParser& defineBool(
      const std::string& longOpt,
      char shortOpt,
      const std::string& helpText);

 private:
  FlagParser& define(
      const std::string& longOpt,
      char shortOpt,
      bool required,
      FlagType type,
      const std::string& helpText,
      const std::string& valuePlaceholder,
      const std::string& defaultValue,
      std::function<bool(const std::string&)> validator);

 public:
  void evaluate(const std::vector<std::string>& args, Flags* out) const;
  void evaluate(int argc, const char* argv[], Flags* out) const;
  Flags evaluate(const std::vector<std::string>& args) const;
  Flags evaluate(int argc, const char* argv[]) const;

  std::string helpText(size_t width = 78, size_t helpTextOffset = 30) const;

 private:
  struct FlagDef;
  std::list<FlagDef> flagDefs_;
};

struct FlagParser::FlagDef {
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
