// This file is part of the "x0" project
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/cli/CLI.h>
#include <xzero-base/cli/Flags.h>
#include <xzero-base/net/IPAddress.h>
#include <xzero-base/RuntimeError.h>
#include <xzero-base/Tokenizer.h>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace xzero {

CLI::CLI()
    : flagDefs_(),
      parametersEnabled_(false),
      parametersPlaceholder_(),
      parametersHelpText_() {
}

CLI& CLI::define(
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

CLI& CLI::defineString(
    const std::string& longOpt,
    char shortOpt,
    bool required,
    const std::string& helpText,
    const std::string& defaultValue,
    std::function<bool(const std::string&)> validator) {

  return define(longOpt, shortOpt, required, FlagType::String, helpText,
                "<string>", defaultValue, validator);
}

CLI& CLI::defineNumber(
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

CLI& CLI::defineFloat(
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

CLI& CLI::defineIPAddress(
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

CLI& CLI::defineBool(
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

CLI& CLI::enableParameters(const std::string& valuePlaceholder,
                                         const std::string& helpText) {
  parametersEnabled_ = true;
  parametersPlaceholder_ = valuePlaceholder;
  parametersHelpText_ = helpText;

  return *this;
}

void CLI::evaluate(const std::vector<std::string>& args, Flags* out) const {
  // POSIX compatible flag parsing (ala getopt_long)

  throw RUNTIME_ERROR("Not Implemented"); // TODO
}

void CLI::evaluate(int argc, const char* argv[], Flags* out) const {
  std::vector<std::string> args;
  for (int i = 1; i < argc; ++i)
    args.push_back(argv[i]);

  evaluate(args, out);
}

Flags CLI::evaluate(const std::vector<std::string>& args) const {
  Flags flags;
  evaluate(args, &flags);
  return flags;
}

Flags CLI::evaluate(int argc, const char* argv[]) const {
  Flags flags;
  evaluate(argc, argv, &flags);
  return flags;
}

std::string CLI::helpText(size_t width,
                          size_t helpTextOffset) const {
  std::stringstream sstr;

  for (const FlagDef& fd: flagDefs_)
    sstr << fd.makeHelpText(width, helpTextOffset);

  if (parametersEnabled_) {
    sstr << std::endl;

    size_t p = sstr.tellp();
    sstr << "    -- " << parametersPlaceholder_;
    size_t column = static_cast<size_t>(sstr.tellp()) - p;

    if (column < helpTextOffset)
      sstr << std::setw(helpTextOffset - column) << ' ';
    else
      sstr << std::endl << std::setw(helpTextOffset) << ' ';

    sstr << parametersHelpText_ << std::endl;
  }

  return sstr.str();
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

std::string CLI::FlagDef::makeHelpText(size_t width,
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
  size_t column = sstr.tellp();
  if (column < helpTextOffset) {
    sstr << std::setw(helpTextOffset - sstr.tellp()) << ' ';
  } else {
    sstr << std::endl << std::setw(helpTextOffset) << ' ';
    column = helpTextOffset;
  }

  // help output with default value hint.
  if (type != FlagType::Bool && !defaultValue.empty()) {
    sstr << wordWrap(helpText + " [" + defaultValue + "]",
                     column, width, helpTextOffset);
  } else {
    sstr << wordWrap(helpText, column, width, helpTextOffset);
  }

  sstr << std::endl;

  return sstr.str();
}

}  // namespace xzero
