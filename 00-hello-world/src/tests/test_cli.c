/* Unit tests for cli_parse: option syntax, scan order, and every syntax error. */

#include "test_support.h"
#include "unit_test.h"

#define TRY "Try 'dp-hello --help' for more information.\n"

/* Checks that scanning `arguments` stops with exactly the diagnostic `expected`. */
static void expect_error(TestContext* context, const char* const arguments[],
                         const char* expected) {
  ParsedArguments parsed = parse_list(arguments);
  char text[256];
  if (EXPECT(context, parsed.action == CLI_USAGE_ERROR)) {
    EXPECT_STRING(context, render_error(&parsed.error, text, sizeof text), expected);
  }
}

/* Checks that scanning `arguments` succeeds with the given winning NAME and its spelling. */
static void expect_name(TestContext* context, const char* const arguments[], const char* name,
                        const char* option) {
  ParsedArguments parsed = parse_list(arguments);
  EXPECT(context, parsed.action == CLI_GREET);
  EXPECT_STRING(context, parsed.name.value, name);
  EXPECT_STRING(context, parsed.name.option, option);
}

static void test_no_arguments_greets_with_defaults(TestContext* context) {
  ParsedArguments parsed = parse_list(NO_ARGS);
  EXPECT(context, parsed.action == CLI_GREET);
  EXPECT(context, parsed.name.value == NULL);
  EXPECT(context, parsed.count.value == NULL);
}

static void test_name_value_forms(TestContext* context) {
  expect_name(context, ARGS("-n", "Ada"), "Ada", "-n");
  expect_name(context, ARGS("-nAda"), "Ada", "-n");
  expect_name(context, ARGS("--name", "Ada Lovelace"), "Ada Lovelace", "--name");
  expect_name(context, ARGS("--name=Linus"), "Linus", "--name");
  expect_name(context, ARGS("--name=a=b"), "a=b", "--name");
}

static void test_name_may_start_with_dash(TestContext* context) {
  expect_name(context, ARGS("--name=-x"), "-x", "--name");
  expect_name(context, ARGS("-n-x"), "-x", "-n");
  expect_name(context, ARGS("--name", "-x"), "-x", "--name");
  expect_name(context, ARGS("-n", "--help"), "--help", "-n");
  expect_name(context, ARGS("-n", "--"), "--", "-n");
}

static void test_empty_values_are_scanned(TestContext* context) {
  expect_name(context, ARGS("--name="), "", "--name");
  expect_name(context, ARGS("-n", ""), "", "-n");
}

static void test_bundled_and_separate_values(TestContext* context) {
  ParsedArguments parsed = parse_list(ARGS("-c2", "-nKen"));
  EXPECT(context, parsed.action == CLI_GREET);
  EXPECT_STRING(context, parsed.count.value, "2");
  EXPECT_STRING(context, parsed.count.option, "-c");
  EXPECT_STRING(context, parsed.name.value, "Ken");
  parsed = parse_list(ARGS("--count=2", "--name", "Ken"));
  EXPECT_STRING(context, parsed.count.value, "2");
  EXPECT_STRING(context, parsed.count.option, "--count");
  EXPECT_STRING(context, parsed.name.value, "Ken");
  parsed = parse_list(ARGS("-cn"));
  EXPECT_STRING(context, parsed.count.value, "n");
  EXPECT(context, parsed.name.value == NULL);
}

static void test_last_occurrence_wins(TestContext* context) {
  expect_name(context, ARGS("-n", "A", "-n", "B"), "B", "-n");
  expect_name(context, ARGS("-n", "A", "--name=B"), "B", "--name");
  ParsedArguments parsed = parse_list(ARGS("-c", "abc", "--count", "3"));
  EXPECT_STRING(context, parsed.count.value, "3");
  EXPECT_STRING(context, parsed.count.option, "--count");
}

static void test_help_and_version_stop_the_scan(TestContext* context) {
  EXPECT(context, parse_list(ARGS("--help")).action == CLI_HELP);
  EXPECT(context, parse_list(ARGS("--version")).action == CLI_VERSION);
  EXPECT(context, parse_list(ARGS("--help", "extra", "-x")).action == CLI_HELP);
  EXPECT(context, parse_list(ARGS("--version", "--help")).action == CLI_VERSION);
  EXPECT(context, parse_list(ARGS("-n", "Ada", "--help")).action == CLI_HELP);
  /* Values are validated after the scan, so an invalid count before --help is not an error. */
  EXPECT(context, parse_list(ARGS("--count", "0", "--help")).action == CLI_HELP);
}

static void test_earlier_syntax_error_beats_help(TestContext* context) {
  expect_error(context, ARGS("-x", "--help"), "dp-hello: invalid option -- 'x'\n" TRY);
  expect_error(context, ARGS("--nme", "--version"), "dp-hello: unrecognized option '--nme'\n" TRY);
  expect_error(context, ARGS("--help=1", "--help"),
               "dp-hello: option '--help' doesn't allow an argument\n" TRY);
  expect_error(context, ARGS("extra", "-x", "--help"), "dp-hello: invalid option -- 'x'\n" TRY);
}

static void test_operands_do_not_beat_help(TestContext* context) {
  EXPECT(context, parse_list(ARGS("extra", "--help")).action == CLI_HELP);
  EXPECT(context, parse_list(ARGS("extra", "--version")).action == CLI_VERSION);
  EXPECT(context, parse_list(ARGS("one", "two", "-n", "Ada", "--help")).action == CLI_HELP);
}

static void test_unknown_short_option(TestContext* context) {
  expect_error(context, ARGS("-x"), "dp-hello: invalid option -- 'x'\n" TRY);
  expect_error(context, ARGS("-xn"), "dp-hello: invalid option -- 'x'\n" TRY);
  expect_error(context, ARGS("-h"), "dp-hello: invalid option -- 'h'\n" TRY);
  expect_error(context, ARGS("-\xc3\xab"), "dp-hello: invalid option -- '\xc3\xab'\n" TRY);
}

static void test_unknown_long_option(TestContext* context) {
  expect_error(context, ARGS("--nme", "Ada"), "dp-hello: unrecognized option '--nme'\n" TRY);
  expect_error(context, ARGS("--nam"), "dp-hello: unrecognized option '--nam'\n" TRY);
  expect_error(context, ARGS("--names=x"), "dp-hello: unrecognized option '--names=x'\n" TRY);
  expect_error(context, ARGS("--hel"), "dp-hello: unrecognized option '--hel'\n" TRY);
  expect_error(context, ARGS("--=x"), "dp-hello: unrecognized option '--=x'\n" TRY);
  expect_error(context, ARGS("---name"), "dp-hello: unrecognized option '---name'\n" TRY);
}

static void test_missing_values(TestContext* context) {
  expect_error(context, ARGS("-n"), "dp-hello: option requires an argument -- 'n'\n" TRY);
  expect_error(context, ARGS("-c"), "dp-hello: option requires an argument -- 'c'\n" TRY);
  expect_error(context, ARGS("--name"), "dp-hello: option '--name' requires an argument\n" TRY);
  expect_error(context, ARGS("--count"), "dp-hello: option '--count' requires an argument\n" TRY);
  expect_error(context, ARGS("-n", "Ada", "-c"),
               "dp-hello: option requires an argument -- 'c'\n" TRY);
}

static void test_flags_reject_values(TestContext* context) {
  expect_error(context, ARGS("--help=1"),
               "dp-hello: option '--help' doesn't allow an argument\n" TRY);
  expect_error(context, ARGS("--version=1"),
               "dp-hello: option '--version' doesn't allow an argument\n" TRY);
  expect_error(context, ARGS("--version="),
               "dp-hello: option '--version' doesn't allow an argument\n" TRY);
}

static void test_operands_are_rejected(TestContext* context) {
  expect_error(context, ARGS("extra"), "dp-hello: extra operand 'extra'\n" TRY);
  expect_error(context, ARGS("-"), "dp-hello: extra operand '-'\n" TRY);
  expect_error(context, ARGS("-n", "Ada", "one", "two"), "dp-hello: extra operand 'one'\n" TRY);
  expect_error(context, ARGS("--", "-n"), "dp-hello: extra operand '-n'\n" TRY);
  expect_error(context, ARGS("--", "--help"), "dp-hello: extra operand '--help'\n" TRY);
  expect_error(context, ARGS("--", "--"), "dp-hello: extra operand '--'\n" TRY);
  expect_error(context, ARGS("one", "-n", "Ada"), "dp-hello: extra operand 'one'\n" TRY);
  expect_error(context, ARGS("--", "--help", "x"), "dp-hello: extra operand '--help'\n" TRY);
}

static void test_terminator_alone_is_accepted(TestContext* context) {
  EXPECT(context, parse_list(ARGS("--")).action == CLI_GREET);
  expect_name(context, ARGS("-n", "Ada", "--"), "Ada", "-n");
}

void run_cli_tests(TestContext* context) {
  RUN_TEST(context, test_no_arguments_greets_with_defaults);
  RUN_TEST(context, test_name_value_forms);
  RUN_TEST(context, test_name_may_start_with_dash);
  RUN_TEST(context, test_empty_values_are_scanned);
  RUN_TEST(context, test_bundled_and_separate_values);
  RUN_TEST(context, test_last_occurrence_wins);
  RUN_TEST(context, test_help_and_version_stop_the_scan);
  RUN_TEST(context, test_earlier_syntax_error_beats_help);
  RUN_TEST(context, test_operands_do_not_beat_help);
  RUN_TEST(context, test_unknown_short_option);
  RUN_TEST(context, test_unknown_long_option);
  RUN_TEST(context, test_missing_values);
  RUN_TEST(context, test_flags_reject_values);
  RUN_TEST(context, test_operands_are_rejected);
  RUN_TEST(context, test_terminator_alone_is_accepted);
}
