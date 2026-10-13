/* A minimal unit-test runner built on plain C checks: no third-party framework. */
#ifndef DP_HELLO_UNIT_TEST_H
#define DP_HELLO_UNIT_TEST_H

#include <stdbool.h>

/* Counters for one run of the suite, passed to every test (no global state). */
typedef struct {
  int tests_run;
  int tests_failed;
  bool current_failed;
} TestContext;

typedef void (*TestFunction)(TestContext* context);

/* Runs one test and prints "ok" or "FAIL" with its name. */
void unit_run(TestContext* context, const char* name, TestFunction test);

/* Records a failed check (with its source location) unless `condition` holds. */
bool unit_check(TestContext* context, bool condition, const char* expression, const char* file,
                int line);

/* Like unit_check for string equality; prints both strings on failure. NULL equals only NULL. */
bool unit_check_string(TestContext* context, const char* actual, const char* expected,
                       const char* expression, const char* file, int line);

#define EXPECT(context, condition) \
  unit_check((context), (condition), #condition, __FILE__, __LINE__)
#define EXPECT_STRING(context, actual, expected) \
  unit_check_string((context), (actual), (expected), #actual, __FILE__, __LINE__)
#define RUN_TEST(context, test) unit_run((context), #test, (test))

/* The suites, one per unit under test. */
void run_cli_tests(TestContext* context);
void run_validation_tests(TestContext* context);
void run_greeting_tests(TestContext* context);
void run_usage_error_tests(TestContext* context);

#endif
