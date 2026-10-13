/* Command-line scanning (CONV-07 ... CONV-12): argv to raw option values or a usage error. */
#ifndef DP_HELLO_CLI_H
#define DP_HELLO_CLI_H

#include <stdbool.h>
#include <stddef.h>

#include "usage_error.h"

/* What the scan decided the invocation should do. */
typedef enum { CLI_GREET, CLI_HELP, CLI_VERSION, CLI_USAGE_ERROR } CliAction;

/*
 * The last occurrence of a value option (CONV-11), not yet validated. `option` is the option
 * as written ("-n" or "--name"), for diagnostics; both are NULL when the option was not given.
 */
typedef struct {
  const char* value;
  const char* option;
} OptionValue;

/* Result of scanning the arguments. `error` is meaningful only when action is CLI_USAGE_ERROR. */
typedef struct {
  CliAction action;
  OptionValue name;
  OptionValue count;
  UsageError error;
} ParsedArguments;

/*
 * Scans `arguments` (the command line without the program name) from left to right. Stops at
 * the first --help or --version, or at the first option syntax error. Operands are collected
 * during the scan and rejected after it (CONV-12), so `extra --help` prints the help. Never
 * allocates; every pointer in the result points into `arguments` or at a string literal.
 */
ParsedArguments cli_parse(size_t argument_count, const char* const arguments[]);

#endif
