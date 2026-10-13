/* Unit tests for parse_count and validate_arguments: partitions and boundaries. */
#include "../dp_hello/validation.h"
#include "test_support.h"
#include "unit_test.h"

#define TRY "Try 'dp-hello --help' for more information.\n"

/* Returns true if `text` is a valid COUNT whose value is `expected`. */
static bool count_is(const char* text, unsigned expected) {
  unsigned count = 0;
  return parse_count(text, &count) && count == expected;
}

static bool count_rejected(const char* text) {
  unsigned count = 42;
  return !parse_count(text, &count) && count == 42;
}

/* Validates the scan of `arguments`; returns the diagnostic, or "" when validation passes. */
static const char* validation_error(const char* const arguments[], GreetingRequest* request,
                                    char* buffer, size_t capacity) {
  ParsedArguments parsed = parse_list(arguments);
  UsageError error;
  if (parsed.action != CLI_GREET) {
    return "(scan did not succeed)";
  }
  if (validate_arguments(&parsed, request, &error)) {
    return "";
  }
  return render_error(&error, buffer, capacity);
}

static void test_count_boundaries(TestContext* context) {
  EXPECT(context, count_rejected("0"));
  EXPECT(context, count_is("1", 1));
  EXPECT(context, count_is("999", 999));
  EXPECT(context, count_is("1000", 1000));
  EXPECT(context, count_rejected("1001"));
  EXPECT(context, count_rejected("4294967297"));
  EXPECT(context, count_rejected("99999999999999999999999999"));
}

static void test_count_leading_zeros(TestContext* context) {
  EXPECT(context, count_is("007", 7));
  EXPECT(context, count_is("0001000", 1000));
  EXPECT(context, count_rejected("000"));
  EXPECT(context, count_rejected("0001001"));
}

static void test_count_non_digits(TestContext* context) {
  EXPECT(context, count_rejected(""));
  EXPECT(context, count_rejected("abc"));
  EXPECT(context, count_rejected("+5"));
  EXPECT(context, count_rejected("-1"));
  EXPECT(context, count_rejected(" 5"));
  EXPECT(context, count_rejected("5 "));
  EXPECT(context, count_rejected("1.0"));
  EXPECT(context, count_rejected("1e3"));
  EXPECT(context, count_rejected("0x10"));
  EXPECT(context, count_rejected("\xd9\xa3")); /* ARABIC-INDIC DIGIT THREE is not ASCII */
}

static void test_defaults(TestContext* context) {
  GreetingRequest request = {NULL, 0};
  char text[256];
  EXPECT_STRING(context, validation_error(NO_ARGS, &request, text, sizeof text), "");
  EXPECT_STRING(context, request.name, "World");
  EXPECT(context, request.count == 1);
}

static void test_valid_values(TestContext* context) {
  GreetingRequest request = {NULL, 0};
  char text[256];
  EXPECT_STRING(context,
                validation_error(ARGS("-n", "Zo\xc3\xab", "-c", "3"), &request, text, sizeof text),
                "");
  EXPECT_STRING(context, request.name, "Zo\xc3\xab");
  EXPECT(context, request.count == 3);
}

static void test_empty_name_rejected(TestContext* context) {
  GreetingRequest request = {NULL, 0};
  char text[256];
  EXPECT_STRING(context, validation_error(ARGS("--name", ""), &request, text, sizeof text),
                "dp-hello: invalid argument '' for '--name'\n" TRY);
  EXPECT_STRING(context, validation_error(ARGS("--name="), &request, text, sizeof text),
                "dp-hello: invalid argument '' for '--name'\n" TRY);
  EXPECT_STRING(context, validation_error(ARGS("-n", ""), &request, text, sizeof text),
                "dp-hello: invalid argument '' for '-n'\n" TRY);
}

static void test_invalid_count_rejected(TestContext* context) {
  GreetingRequest request = {NULL, 0};
  char text[256];
  EXPECT_STRING(context, validation_error(ARGS("--count", "0"), &request, text, sizeof text),
                "dp-hello: invalid argument '0' for '--count'\n" TRY);
  EXPECT_STRING(context, validation_error(ARGS("-c", "abc"), &request, text, sizeof text),
                "dp-hello: invalid argument 'abc' for '-c'\n" TRY);
  EXPECT_STRING(context, validation_error(ARGS("-c1001"), &request, text, sizeof text),
                "dp-hello: invalid argument '1001' for '-c'\n" TRY);
  EXPECT_STRING(context, validation_error(ARGS("--count="), &request, text, sizeof text),
                "dp-hello: invalid argument '' for '--count'\n" TRY);
}

static void test_only_the_winning_value_is_validated(TestContext* context) {
  GreetingRequest request = {NULL, 0};
  char text[256];
  EXPECT_STRING(context,
                validation_error(ARGS("-n", "", "-n", "B", "-c", "0", "-c", "2"), &request, text,
                                 sizeof text),
                "");
  EXPECT_STRING(context, request.name, "B");
  EXPECT(context, request.count == 2);
  EXPECT_STRING(context,
                validation_error(ARGS("-c", "2", "--count", "x"), &request, text, sizeof text),
                "dp-hello: invalid argument 'x' for '--count'\n" TRY);
}

static void test_name_is_validated_before_count(TestContext* context) {
  GreetingRequest request = {NULL, 0};
  char text[256];
  EXPECT_STRING(context, validation_error(ARGS("-c", "0", "-n", ""), &request, text, sizeof text),
                "dp-hello: invalid argument '' for '-n'\n" TRY);
}

void run_validation_tests(TestContext* context) {
  RUN_TEST(context, test_count_boundaries);
  RUN_TEST(context, test_count_leading_zeros);
  RUN_TEST(context, test_count_non_digits);
  RUN_TEST(context, test_defaults);
  RUN_TEST(context, test_valid_values);
  RUN_TEST(context, test_empty_name_rejected);
  RUN_TEST(context, test_invalid_count_rejected);
  RUN_TEST(context, test_only_the_winning_value_is_validated);
  RUN_TEST(context, test_name_is_validated_before_count);
}
