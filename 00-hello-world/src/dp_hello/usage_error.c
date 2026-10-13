#include "usage_error.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "program.h"

/* Every diagnostic starts with the canonical name (CONV-01) and ends with this line (CONV-14). */
#define PREFIX PROGRAM_NAME ": "
#define TRY_HELP "Try '" PROGRAM_NAME " --help' for more information.\n"

UsageError usage_error_make(UsageErrorKind kind, const char* subject, const char* option) {
  UsageError error = {kind, subject, strlen(subject), option};
  return error;
}

int usage_error_format(char* buffer, size_t capacity, const UsageError* error) {
  if (error->subject_length > INT_MAX) {
    return -1;
  }
  int length = (int)error->subject_length;
  const char* subject = error->subject;
  switch (error->kind) {
    case USAGE_INVALID_OPTION:
      return snprintf(buffer, capacity, PREFIX "invalid option -- '%.*s'\n" TRY_HELP, length,
                      subject);
    case USAGE_UNRECOGNIZED_OPTION:
      return snprintf(buffer, capacity, PREFIX "unrecognized option '%.*s'\n" TRY_HELP, length,
                      subject);
    case USAGE_SHORT_MISSING_VALUE:
      return snprintf(buffer, capacity, PREFIX "option requires an argument -- '%.*s'\n" TRY_HELP,
                      length, subject);
    case USAGE_LONG_MISSING_VALUE:
      return snprintf(buffer, capacity, PREFIX "option '%.*s' requires an argument\n" TRY_HELP,
                      length, subject);
    case USAGE_LONG_UNEXPECTED_VALUE:
      return snprintf(buffer, capacity, PREFIX "option '%.*s' doesn't allow an argument\n" TRY_HELP,
                      length, subject);
    case USAGE_INVALID_VALUE:
      return snprintf(buffer, capacity, PREFIX "invalid argument '%.*s' for '%s'\n" TRY_HELP,
                      length, subject, error->option != NULL ? error->option : "");
    case USAGE_EXTRA_OPERAND:
      return snprintf(buffer, capacity, PREFIX "extra operand '%.*s'\n" TRY_HELP, length, subject);
  }
  return -1;
}
