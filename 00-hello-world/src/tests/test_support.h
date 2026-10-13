/* Helpers shared by the suites: argument lists and rendered diagnostics. */
#ifndef DP_HELLO_TEST_SUPPORT_H
#define DP_HELLO_TEST_SUPPORT_H

#include <stddef.h>

#include "../dp_hello/cli.h"
#include "../dp_hello/usage_error.h"

/* A NULL-terminated argument list (without the program name), and the empty list. */
#define ARGS(...) ((const char* const[]){__VA_ARGS__, NULL})
#define NO_ARGS ((const char* const[]){NULL})

/* Scans a NULL-terminated argument list. */
ParsedArguments parse_list(const char* const arguments[]);

/* Renders `error` into `buffer` (truncated to `capacity`) and returns `buffer`. */
const char* render_error(const UsageError* error, char* buffer, size_t capacity);

#endif
