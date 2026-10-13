/* Unit tests for usage_error_format and the fixed help and version texts. */
#include <string.h>

#include "../dp_hello/output.h"
#include "../dp_hello/usage_error.h"
#include "test_support.h"
#include "unit_test.h"

#define TRY "Try 'dp-hello --help' for more information.\n"

static void expect_message(TestContext* context, UsageError error, const char* expected) {
  char text[256];
  EXPECT_STRING(context, render_error(&error, text, sizeof text), expected);
}

static void test_every_message_format(TestContext* context) {
  expect_message(context, usage_error_make(USAGE_INVALID_OPTION, "x", NULL),
                 "dp-hello: invalid option -- 'x'\n" TRY);
  expect_message(context, usage_error_make(USAGE_UNRECOGNIZED_OPTION, "--nme", NULL),
                 "dp-hello: unrecognized option '--nme'\n" TRY);
  expect_message(context, usage_error_make(USAGE_SHORT_MISSING_VALUE, "n", NULL),
                 "dp-hello: option requires an argument -- 'n'\n" TRY);
  expect_message(context, usage_error_make(USAGE_LONG_MISSING_VALUE, "--count", NULL),
                 "dp-hello: option '--count' requires an argument\n" TRY);
  expect_message(context, usage_error_make(USAGE_LONG_UNEXPECTED_VALUE, "--help", NULL),
                 "dp-hello: option '--help' doesn't allow an argument\n" TRY);
  expect_message(context, usage_error_make(USAGE_INVALID_VALUE, "0", "--count"),
                 "dp-hello: invalid argument '0' for '--count'\n" TRY);
  expect_message(context, usage_error_make(USAGE_EXTRA_OPERAND, "foo", NULL),
                 "dp-hello: extra operand 'foo'\n" TRY);
}

static void test_values_are_not_escaped(TestContext* context) {
  expect_message(context, usage_error_make(USAGE_EXTRA_OPERAND, "it's %s", NULL),
                 "dp-hello: extra operand 'it's %s'\n" TRY);
  expect_message(context, usage_error_make(USAGE_INVALID_VALUE, "", "-n"),
                 "dp-hello: invalid argument '' for '-n'\n" TRY);
}

static void test_subject_length_is_respected(TestContext* context) {
  UsageError error = {USAGE_INVALID_OPTION, "xyz", 1, NULL};
  expect_message(context, error, "dp-hello: invalid option -- 'x'\n" TRY);
}

static void test_version_text(TestContext* context) {
  EXPECT_STRING(context, VERSION_TEXT, "dp-hello (dialectic-playground) 1.0.0\n");
}

static void test_help_text_layout(TestContext* context) {
  EXPECT(context, strncmp(HELP_TEXT, "Usage: dp-hello [OPTION]...\n", 28) == 0);
  size_t length = strlen(HELP_TEXT);
  EXPECT(context, length > 0 && HELP_TEXT[length - 1] == '\n');
  EXPECT(context, strchr(HELP_TEXT, '\r') == NULL);
}

void run_usage_error_tests(TestContext* context) {
  RUN_TEST(context, test_every_message_format);
  RUN_TEST(context, test_values_are_not_escaped);
  RUN_TEST(context, test_subject_length_is_respected);
  RUN_TEST(context, test_version_text);
  RUN_TEST(context, test_help_text_layout);
}
