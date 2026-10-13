/* Output: the only unit that writes to the standard streams (FR-01, FR-07, FR-08, CONV-14). */
#ifndef DP_HELLO_OUTPUT_H
#define DP_HELLO_OUTPUT_H

#include <stdbool.h>
#include <stdio.h>

#include "usage_error.h"
#include "validation.h"

/* The help text from the user manual, byte for byte (CONV-23). */
extern const char HELP_TEXT[];

/* The version line (CONV-02). */
extern const char VERSION_TEXT[];

/* Writes `text` to `stream` and flushes it; returns false if any byte could not be written. */
bool output_text(FILE* stream, const char* text);

/* Writes the greeting line request->count times; returns false on a write failure. */
bool output_greetings(FILE* stream, const GreetingRequest* request);

/* Writes the two-line diagnostic for `error`. */
void output_usage_error(FILE* stream, const UsageError* error);

#endif
