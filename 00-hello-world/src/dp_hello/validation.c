#include "validation.h"

#include <stddef.h>

bool parse_count(const char* text, unsigned* count) {
  if (text[0] == '\0') {
    return false;
  }
  unsigned value = 0;
  for (const char* digit = text; *digit != '\0'; digit++) {
    if (*digit < '0' || *digit > '9') {
      return false;
    }
    /* Saturate above the maximum so long digit strings cannot overflow. */
    if (value <= MAXIMUM_COUNT) {
      value = value * 10u + (unsigned)(*digit - '0');
    }
  }
  if (value < MINIMUM_COUNT || value > MAXIMUM_COUNT) {
    return false;
  }
  *count = value;
  return true;
}

bool validate_arguments(const ParsedArguments* arguments, GreetingRequest* request,
                        UsageError* error) {
  const OptionValue* name = &arguments->name;
  const OptionValue* count = &arguments->count;
  GreetingRequest validated = {DEFAULT_NAME, DEFAULT_COUNT};
  if (name->value != NULL) {
    if (name->value[0] == '\0') {
      *error = usage_error_make(USAGE_INVALID_VALUE, name->value, name->option);
      return false;
    }
    validated.name = name->value;
  }
  if (count->value != NULL && !parse_count(count->value, &validated.count)) {
    *error = usage_error_make(USAGE_INVALID_VALUE, count->value, count->option);
    return false;
  }
  *request = validated;
  return true;
}
