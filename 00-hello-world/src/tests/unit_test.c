#include "unit_test.h"

#include <stdio.h>
#include <string.h>

void unit_run(TestContext* context, const char* name, TestFunction test) {
  context->current_failed = false;
  test(context);
  context->tests_run++;
  if (context->current_failed) {
    context->tests_failed++;
  }
  printf("%s %s\n", context->current_failed ? "FAIL" : "ok  ", name);
}

bool unit_check(TestContext* context, bool condition, const char* expression, const char* file,
                int line) {
  if (!condition) {
    context->current_failed = true;
    printf("  %s:%d: check failed: %s\n", file, line, expression);
  }
  return condition;
}

bool unit_check_string(TestContext* context, const char* actual, const char* expected,
                       const char* expression, const char* file, int line) {
  bool equal =
      (actual == NULL || expected == NULL) ? actual == expected : strcmp(actual, expected) == 0;
  if (!equal) {
    context->current_failed = true;
    printf("  %s:%d: %s\n    actual:   [%s]\n    expected: [%s]\n", file, line, expression,
           actual != NULL ? actual : "(null)", expected != NULL ? expected : "(null)");
  }
  return equal;
}

int main(void) {
  TestContext context = {0, 0, false};
  run_cli_tests(&context);
  run_validation_tests(&context);
  run_greeting_tests(&context);
  run_usage_error_tests(&context);
  printf("%d tests, %d failed\n", context.tests_run, context.tests_failed);
  return context.tests_failed == 0 ? 0 : 1;
}
