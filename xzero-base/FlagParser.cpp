// This file is part of the "x0" project
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/FlagParser.h>
#include <xzero-base/net/IPAddress.h>
#include <xzero-base/RuntimeError.h>
#include <xzero-base/Tokenizer.h>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace xzero {

// {{{ Flags impl
Flags::Flags() {
}

std::string Flags::getString(const std::string& flag) const {
  auto i = set_.find(flag);
  if (i == set_.end())
    return "";

  return i->second.second;
}

long int Flags::getNumber(const std::string& flag) const {
  auto i = set_.find(flag);
  if (i == set_.end())
    return 0;

  return std::stoi(i->second.second);
}

float Flags::getFloat(const std::string& flag) const {
  auto i = set_.find(flag);
  if (i == set_.end())
    return 0.0f;

  return std::stof(i->second.second);
}

bool Flags::getBool(const std::string& flag) const {
  auto i = set_.find(flag);
  if (i == set_.end())
    return false;

  return !i->second.second.empty();
}
// }}}

FlagParser& FlagParser::define(
    const std::string& longOpt,
    char shortOpt,
    bool required,
    FlagType type,
    const std::string& helpText,
    const std::string& valuePlaceholder,
    const std::string& defaultValue,
    std::function<bool(const std::string&)> validator) {

  FlagDef fd;
  fd.type = type;
  fd.longOption = longOpt;
  fd.shortOption = shortOpt;
  fd.required = required;
  fd.helpText = helpText;
  fd.valuePlaceholder = valuePlaceholder;
  fd.defaultValue = defaultValue;
  fd.validator = validator;

  flagDefs_.emplace_back(fd);

  return *this;
}

FlagParser& FlagParser::defineString(
    const std::string& longOpt,
    char shortOpt,
    bool required,
    const std::string& helpText,
    const std::string& defaultValue,
    std::function<bool(const std::string&)> validator) {

  return define(longOpt, shortOpt, required, FlagType::String, helpText,
                "<string>", defaultValue, validator);
}

FlagParser& FlagParser::defineNumber(
    const std::string& longOpt,
    char shortOpt,
    bool required,
    const std::string& helpText,
    long int defaultValue,
    std::function<bool(long int)> validator) {

  return define(
      longOpt, shortOpt, required, FlagType::Number, helpText,
      "<int>", std::to_string(defaultValue),
      [=](const std::string& value) -> bool {
        return validator ? validator(std::stoi(value)) : true;
      });
}

FlagParser& FlagParser::defineFloat(
    const std::string& longOpt,
    char shortOpt,
    bool required,
    const std::string& helpText,
    float defaultValue,
    std::function<bool(float)> validator) {

  return define(
      longOpt, shortOpt, required, FlagType::Float, helpText,
      "<float>", std::to_string(defaultValue),
      [=](const std::string& value) -> bool {
        return validator ? validator(std::stof(value)) : true;
      });
}

FlagParser& FlagParser::defineIPAddress(
    const std::string& longOpt,
    char shortOpt,
    bool required,
    const std::string& helpText,
    const IPAddress& defaultValue,
    std::function<bool(const IPAddress&)> validator) {

  return define(
      longOpt, shortOpt, required, FlagType::IP, helpText,
      "<IP>", defaultValue.str(),
      [=](const std::string& value) -> bool {
        return validator ? validator(IPAddress(value)) : true;
      });
}

FlagParser& FlagParser::defineBool(
    const std::string& longOpt,
    char shortOpt,
    const std::string& helpText) {

  return define(
      longOpt, shortOpt, false, FlagType::Bool, helpText,
      "<bool>", "false",
      [=](const std::string& value) -> bool {
        return value == "true" || value == "";
      });
}

void FlagParser::evaluate(const std::vector<std::string>& args, Flags* out) const {
  throw RUNTIME_ERROR("Not Implemented"); // TODO
}

void FlagParser::evaluate(int argc, const char* argv[], Flags* out) const {
  std::vector<std::string> args;
  for (int i = 1; i < argc; ++i)
    args.push_back(argv[i]);

  evaluate(args, out);
}

Flags FlagParser::evaluate(const std::vector<std::string>& args) const {
  Flags flags;
  evaluate(args, &flags);
  return flags;
}

Flags FlagParser::evaluate(int argc, const char* argv[]) const {
  Flags flags;
  evaluate(argc, argv, &flags);
  return flags;
}

std::string FlagParser::helpText(size_t width,
                                 size_t helpTextOffset) const {
  std::stringstream help;

  for (const FlagDef& fd: flagDefs_)
    help << fd.makeHelpText(width, helpTextOffset);

  return help.str();
}

static std::string wordWrap(
    const std::string& text,
    size_t currentWidth,
    size_t width,
    size_t indent) {

  //auto words = Tokenizer<std::string, std::string>::tokenize(text, " ");

  std::stringstream sstr;

  size_t i = 0;
  while (i < text.size()) {
    if (currentWidth >= width) {
      sstr << std::endl << std::setw(indent) << ' ';
      currentWidth = 0;
    }

    sstr << text[i];
    currentWidth++;
    i++;
  }

  return sstr.str();
}

std::string FlagParser::FlagDef::makeHelpText(size_t width,
                                              size_t helpTextOffset) const {
  std::stringstream sstr;

  sstr << ' ';

  // short option
  if (shortOption)
    sstr << "-" << shortOption << ", ";
  else
    sstr << "    ";

  // long option
  sstr << "--" << longOption;

  // value placeholder
  if (type != FlagType::Bool) {
    if (!valuePlaceholder.empty()) {
      sstr << "=" << valuePlaceholder;
    } else {
      sstr << "=VALUE";
    }
  }

  // spacer
  if (sstr.tellp() < helpTextOffset)
    sstr << std::setw(helpTextOffset - sstr.tellp()) << ' ';
  else
    sstr << std::endl << std::setw(helpTextOffset) << ' ';

  // help output with default value hint.
  sstr << wordWrap(helpText, sstr.tellp(), width, helpTextOffset);
  if (type != FlagType::Bool && !defaultValue.empty()) {
    if (size_t(sstr.tellp()) + defaultValue.size() + 3 >= width)
      sstr << std::endl << std::setw(helpTextOffset) << ' ';

    sstr << " [" << defaultValue << "]";
  }

  sstr << std::endl;

  return sstr.str();
}

}  // namespace xzero
