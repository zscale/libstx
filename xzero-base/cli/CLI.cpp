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
    std::function<void(const std::string&)> callback) {

  FlagDef fd;
  fd.type = type;
  fd.longOption = longOpt;
  fd.shortOption = shortOpt;
  fd.required = required;
  fd.helpText = helpText;
  fd.valuePlaceholder = valuePlaceholder;
  fd.defaultValue = defaultValue;
  fd.callback = callback;

  flagDefs_.emplace_back(fd);

  return *this;
}

CLI& CLI::defineString(
    const std::string& longOpt,
    char shortOpt,
    const std::string& helpText,
    std::function<void(const std::string&)> callback) {

  return define(longOpt, shortOpt, true, FlagType::String, helpText,
                "<string>", "", callback);
}

CLI& CLI::defineString(
    const std::string& longOpt,
    char shortOpt,
    const std::string& helpText,
    const std::string& defaultValue,
    std::function<void(const std::string&)> callback) {

  return define(longOpt, shortOpt, false, FlagType::String, helpText,
                "<string>", defaultValue, callback);
}

CLI& CLI::defineNumber(
    const std::string& longOpt,
    char shortOpt,
    const std::string& helpText,
    std::function<void(long int)> callback) {

  return define(
      longOpt, shortOpt, true, FlagType::Number, helpText,
      "<int>", "",
      [=](const std::string& value) {
        if (callback) {
          callback(std::stoi(value));
        }
      });
}

CLI& CLI::defineNumber(
    const std::string& longOpt,
    char shortOpt,
    const std::string& helpText,
    long int defaultValue,
    std::function<void(long int)> callback) {

  return define(
      longOpt, shortOpt, false, FlagType::Number, helpText,
      "<int>", std::to_string(defaultValue),
      [=](const std::string& value) {
        if (callback) {
          callback(std::stoi(value));
        }
      });
}

CLI& CLI::defineFloat(
    const std::string& longOpt,
    char shortOpt,
    const std::string& helpText,
    std::function<void(float)> callback) {

  return define(
      longOpt, shortOpt, true, FlagType::Float, helpText,
      "<float>", "",
      [=](const std::string& value) {
        if (callback) {
          callback(std::stof(value));
        }
      });
}

CLI& CLI::defineFloat(
    const std::string& longOpt,
    char shortOpt,
    const std::string& helpText,
    float defaultValue,
    std::function<void(float)> callback) {

  return define(
      longOpt, shortOpt, false, FlagType::Float, helpText,
      "<float>", std::to_string(defaultValue),
      [=](const std::string& value) {
        if (callback) {
          callback(std::stof(value));
        }
      });
}

CLI& CLI::defineIPAddress(
    const std::string& longOpt,
    char shortOpt,
    const std::string& helpText,
    std::function<void(const IPAddress&)> callback) {

  return define(
      longOpt, shortOpt, true, FlagType::IP, helpText,
      "<IP>", "",
      [=](const std::string& value) {
        if (callback) {
          callback(IPAddress(value));
        }
      });
}

CLI& CLI::defineIPAddress(
    const std::string& longOpt,
    char shortOpt,
    const std::string& helpText,
    const IPAddress& defaultValue,
    std::function<void(const IPAddress&)> callback) {

  return define(
      longOpt, shortOpt, false, FlagType::IP, helpText,
      "<IP>", defaultValue.str(),
      [=](const std::string& value) {
        if (callback) {
          callback(IPAddress(value));
        }
      });
}

CLI& CLI::defineBool(
    const std::string& longOpt,
    char shortOpt,
    const std::string& helpText,
    std::function<void(bool)> callback) {

  return define(
      longOpt, shortOpt, false, FlagType::Bool, helpText,
      "<bool>", "",
      [=](const std::string& value) {
        if (callback) {
          callback(value == "true");
        }
      });
}

CLI& CLI::enableParameters(const std::string& valuePlaceholder,
                                         const std::string& helpText) {
  parametersEnabled_ = true;
  parametersPlaceholder_ = valuePlaceholder;
  parametersHelpText_ = helpText;

  return *this;
}

const CLI::FlagDef* CLI::find(const std::string& longOption) const {
  for (const auto& flag: flagDefs_)
    if (flag.longOption == longOption)
      return &flag;

  return nullptr;
}

const CLI::FlagDef* CLI::find(char shortOption) const {
  for (const auto& flag: flagDefs_)
    if (flag.shortOption == shortOption)
      return &flag;

  return nullptr;
}

// -----------------------------------------------------------------------------
static inline FlagType flagTypeOf(const std::string& longOption, const CLI* cli) {
  if (const CLI::FlagDef* fd = cli->find(longOption))
    return fd->type;

  throw CLI::UnknownOptionError("--" + longOption, __FILE__, __LINE__);
}

Flags CLI::evaluate(int argc, const char* argv[]) const {
  std::vector<std::string> args;
  for (int i = 1; i < argc; ++i)
    args.push_back(argv[i]);

  return evaluate(args);
}

Flags CLI::evaluate(const std::vector<std::string>& args) const {
  Flags flags;

  enum class ParsingState {
    Options,
    Parameters,
  };

  ParsingState pstate = ParsingState::Options;
  size_t i = 0;
  while (i < args.size()) {
    std::string arg = args[i];
    if (pstate == ParsingState::Parameters) {
      flags.set("", arg, FlagStyle::UnnamedParameter, FlagType::String);
      i++;
    } else if (arg == "--") {
      pstate = ParsingState::Parameters;
      i++;
    } else if (arg.size() > 2 && arg[0] == '-' && arg[1] == '-') {
      // longopt
      arg = arg.substr(2);
      size_t eq = arg.find('=');
      if (eq != arg.npos) { // --name=value
        std::string name = arg.substr(0, eq);
        std::string value = arg.substr(eq + 1);
        FlagType ft = flagTypeOf(name, this);
        flags.set(name, value, FlagStyle::LongWithValue, ft);
        i++;
      } else { // --name [VALUE]
        FlagType ft = flagTypeOf(arg, this);
        if (ft == FlagType::Bool) { // --name
          flags.set(arg, "", FlagStyle::LongSwitch, ft);
          i++;
        } else { // --name VALUE
          i++;
          std::string name = arg;

          if (i >= args.size())
            throw MissingOptionValueError("--" + name, __FILE__, __LINE__);

          std::string value = args[i];
          i++;

          flags.set(name, value, FlagStyle::LongWithValue, ft);
        }
      }
    } else if (arg.size() > 1 && arg[0] == '-') {
      // shortopt
      arg = arg.substr(1);
      while (!arg.empty()) {
        const FlagDef* fd = find(arg[0]);
        if (fd == nullptr) { // option not found
          throw CLI::UnknownOptionError("-" + arg.substr(0, 1), __FILE__, __LINE__);
        } else if (fd->type == FlagType::Bool) {
          flags.set(fd->longOption, "",
                    FlagStyle::ShortSwitch, FlagType::Bool);
          arg = arg.substr(1);
          i++;
        } else if (arg.size() > 1) { // -fVALUE
          flags.set(fd->longOption, arg.substr(1),
                    FlagStyle::ShortSwitch, fd->type);
          arg.clear();
          i++;
        } else { // -f VALUE
          std::string name = fd->longOption;
          i++;

          if (i >= args.size()) {
            char buf[3] = { '-', fd->shortOption, '\0' };
            throw MissingOptionValueError(buf, __FILE__, __LINE__);
          }

          arg.clear();
          std::string value = args[i];
          i++;

          if (!value.empty() && value[0] == '-') {
            char buf[3] = { '-', fd->shortOption, '\0' };
            throw MissingOptionValueError(buf, __FILE__, __LINE__);
          }

          flags.set(name, value, FlagStyle::ShortSwitch, fd->type);
        }
      }
    } else {
      // oops
      char buf[128];
      snprintf(buf, sizeof(buf), "Option \"%s\" could not be parsed.", args[i].c_str());
      throw ValidationError(buf, __FILE__, __LINE__);
    }
  }

  // fill any missing default flags
  for (const FlagDef& fd: flagDefs_) {
    if (!fd.defaultValue.empty()) {
      if (!flags.isSet(fd.longOption)) {
        flags.set(fd.longOption, fd.defaultValue,
                  FlagStyle::LongWithValue, fd.type);
      }
    }
  }

  return flags;
}

// -----------------------------------------------------------------------------

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

// {{{ CLI::FlagDef
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
// }}}

}  // namespace xzero
