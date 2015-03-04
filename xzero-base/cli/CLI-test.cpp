// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/cli/CLI.h>
#include <xzero-base/cli/Flags.h>
#include <xzero-base/net/IPAddress.h>
#include <gtest/gtest.h>

using namespace xzero;

/**
 * cmdline test cases
 *
 * - explicit boolean (long option)
 * - explicit boolean (short option)
 * - explicit short option, single
 * - explicit short option, first in chain
 * - explicit short option, deep in chain
 * - defaulting long-option
 * - defaulting long-option
 * - required long-option with value
 * - required short-option with value
 * - errors
 *   - raise on unknown long option
 *   - raise on unknown short option
 *   - raise on missing long-option value
 *   - raise on missing short-option value
 *   - raise on missing required option
 */

TEST(CLI, posix_cmdline) {
  CLI cli;
  cli.defineBool("some", 's', "Something");
  Flags flags = cli.evaluate({"--some"});
}

TEST(CLI, defaults) {
  CLI cli;
  cli.defineString("some", 's', "Something", "some value");
  cli.defineBool("bool", 'b', "some boolean");

  Flags flags = cli.evaluate({});

  ASSERT_EQ(1, flags.size());
  ASSERT_TRUE(flags.isSet("some"));
  ASSERT_EQ("some value", flags.getString("some"));

  ASSERT_FALSE(flags.isSet("bool"));
}

TEST(CLI, raise_on_unknown_long_option) {
  CLI cli;
  cli.defineBool("some", 's', "Something");
  ASSERT_THROW(cli.evaluate({"--something-else"}), CLI::UnknownOptionError);
}

TEST(CLI, raise_on_unknown_short_option) {
  CLI cli;
  cli.defineBool("some", 's', "Something");
  ASSERT_THROW(cli.evaluate({"-t"}), CLI::UnknownOptionError);
}

TEST(CLI, long_option_spaced_value) {
  CLI cli;
  cli.defineString("some", 's', "Something");
  Flags f = cli.evaluate({"--some", "bugs"});
  ASSERT_EQ("bugs", f.getString("some"));
}

TEST(CLI, raise_on_missing_long_option) {
  CLI cli;
  cli.defineString("some", 's', "Something");
  ASSERT_THROW(cli.evaluate({"--some"}), CLI::MissingOptionValueError);
}

TEST(CLI, raise_on_missing_option_value) {
  CLI cli;
  cli.defineString("some", 's', "Something");
  cli.defineString("tea", 't', "Tea Time");
  ASSERT_THROW(cli.evaluate({"-s", "-tblack"}), CLI::MissingOptionValueError);
  ASSERT_THROW(cli.evaluate({"-swhite", "-t"}), CLI::MissingOptionValueError);
}

TEST(CLI, short_option_values) {
  CLI cli;
  cli.defineString("some", 's', "Something");
  cli.defineString("tea", 't', "Tea Time");

  Flags flags = cli.evaluate({"-sthing", "-t", "time"});

  ASSERT_EQ(2, flags.size());
  ASSERT_EQ("thing", flags.getString("some"));
  ASSERT_EQ("time", flags.getString("time"));
}
