#include "greeting.h"

#include <stdio.h>

int greeting_format(char* buffer, size_t capacity, const char* name) {
  return snprintf(buffer, capacity, "Hello, %s!\n", name);
}
