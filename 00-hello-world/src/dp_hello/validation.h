/* Validation of the winning option values (FR-03, FR-04, FR-05, FR-06). */
#ifndef DP_HELLO_VALIDATION_H
#define DP_HELLO_VALIDATION_H

#include <stdbool.h>

#include "cli.h"
#include "usage_error.h"

#define DEFAULT_NAME "World"
#define DEFAULT_COUNT 1u
#define MINIMUM_COUNT 1u
#define MAXIMUM_COUNT 1000u

/* A validated request: a non-empty NAME and a COUNT in MINIMUM_COUNT ... MAXIMUM_COUNT. */
typedef struct {
  const char* name;
  unsigned count;
} GreetingRequest;

/*
 * Parses COUNT text: one or more ASCII digits (leading zeros allowed) whose value is within
 * MINIMUM_COUNT ... MAXIMUM_COUNT. Stores the value in `*count` and returns true, or returns
 * false without touching `*count`.
 */
bool parse_count(const char* text, unsigned* count);

/*
 * Turns the scanned (last-wins) values into a request, applying the defaults. On an empty
 * NAME or an invalid COUNT, stores the USAGE_INVALID_VALUE error and returns false. NAME is
 * checked before COUNT.
 */
bool validate_arguments(const ParsedArguments* arguments, GreetingRequest* request,
                        UsageError* error);

#endif
