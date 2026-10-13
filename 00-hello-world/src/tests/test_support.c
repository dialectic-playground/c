#include "test_support.h"

#include <string.h>

ParsedArguments parse_list(const char* const arguments[]) {
  size_t count = 0;
  while (arguments[count] != NULL) {
    count++;
  }
  return cli_parse(count, arguments);
}

const char* render_error(const UsageError* error, char* buffer, size_t capacity) {
  if (usage_error_format(buffer, capacity, error) < 0 && capacity > 0) {
    buffer[0] = '\0';
  }
  return buffer;
}
