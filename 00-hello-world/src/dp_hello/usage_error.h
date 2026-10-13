/* Usage errors (CONV-13, CONV-14) and their exact diagnostic text. */
#ifndef DP_HELLO_USAGE_ERROR_H
#define DP_HELLO_USAGE_ERROR_H

#include <stddef.h>

/* Every usage error dp-hello can report; each maps to one CONV-13 message. */
typedef enum {
  USAGE_INVALID_OPTION,        /* unknown short option: subject is the option character */
  USAGE_UNRECOGNIZED_OPTION,   /* unknown long option: subject is the whole token */
  USAGE_SHORT_MISSING_VALUE,   /* -n / -c without a value: subject is the letter */
  USAGE_LONG_MISSING_VALUE,    /* --name / --count without a value: subject is "--name" */
  USAGE_LONG_UNEXPECTED_VALUE, /* --help=... / --version=...: subject is "--help" */
  USAGE_INVALID_VALUE,         /* bad NAME or COUNT: subject is the value, option as written */
  USAGE_EXTRA_OPERAND          /* any operand: subject is the first operand */
} UsageErrorKind;

/*
 * One usage error. `subject` points into the argument list (or a string literal) and is
 * `subject_length` bytes long, so a multi-byte UTF-8 option character can be quoted whole.
 * `option` is only used by USAGE_INVALID_VALUE: the option as the user wrote it ("-n").
 */
typedef struct {
  UsageErrorKind kind;
  const char* subject;
  size_t subject_length;
  const char* option;
} UsageError;

/* Builds a UsageError whose subject is the whole NUL-terminated string `subject`. */
UsageError usage_error_make(UsageErrorKind kind, const char* subject, const char* option);

/*
 * Formats the complete two-line diagnostic (message line and the "Try ..." line, each ending
 * in LF) like snprintf: writes at most `capacity` bytes including the terminating NUL and
 * returns the length the full text needs, or a negative value on an encoding error.
 */
int usage_error_format(char* buffer, size_t capacity, const UsageError* error);

#endif
