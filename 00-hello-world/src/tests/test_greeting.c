/* Unit tests for greeting_format, the pure greeting construction (NFR-05). */
#include <string.h>

#include "../dp_hello/greeting.h"
#include "unit_test.h"

/* Formats the greeting for `name` into `buffer`; returns the reported length. */
static int greet(const char* name, char* buffer, size_t capacity) {
  return greeting_format(buffer, capacity, name);
}

static void test_default_name(TestContext* context) {
  char line[64];
  EXPECT(context, greet("World", line, sizeof line) == 14);
  EXPECT_STRING(context, line, "Hello, World!\n");
}

static void test_name_is_copied_verbatim(TestContext* context) {
  char line[64];
  greet("Ada Lovelace", line, sizeof line);
  EXPECT_STRING(context, line, "Hello, Ada Lovelace!\n");
  greet("Hi!", line, sizeof line);
  EXPECT_STRING(context, line, "Hello, Hi!!\n");
  greet("-x", line, sizeof line);
  EXPECT_STRING(context, line, "Hello, -x!\n");
  greet("100%s %d", line, sizeof line);
  EXPECT_STRING(context, line, "Hello, 100%s %d!\n");
}

static void test_utf8_name_is_byte_exact(TestContext* context) {
  char line[64];
  EXPECT(context, greet("Zo\xc3\xab", line, sizeof line) == 13);
  EXPECT(context, memcmp(line, "Hello, Zo\xc3\xab!\n", 14) == 0);
}

static void test_length_query_and_truncation(TestContext* context) {
  EXPECT(context, greeting_format(NULL, 0, "Ada") == 12);
  char small[5] = {'x', 'x', 'x', 'x', 'x'};
  EXPECT(context, greet("Ada", small, sizeof small) == 12);
  EXPECT_STRING(context, small, "Hell");
}

static void test_is_pure(TestContext* context) {
  char first[64];
  char second[64];
  greet("Grace", first, sizeof first);
  greet("Grace", second, sizeof second);
  EXPECT_STRING(context, first, second);
}

void run_greeting_tests(TestContext* context) {
  RUN_TEST(context, test_default_name);
  RUN_TEST(context, test_name_is_copied_verbatim);
  RUN_TEST(context, test_utf8_name_is_byte_exact);
  RUN_TEST(context, test_length_query_and_truncation);
  RUN_TEST(context, test_is_pure);
}
