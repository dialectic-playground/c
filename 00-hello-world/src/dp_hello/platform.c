#include "platform.h"

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

void platform_use_binary_streams(void) {
  (void)_setmode(_fileno(stdout), _O_BINARY);
  (void)_setmode(_fileno(stderr), _O_BINARY);
}

/* Converts one NUL-terminated UTF-16 string to a newly allocated UTF-8 string. */
static char* utf16_to_utf8(const wchar_t* wide) {
  int size = WideCharToMultiByte(CP_UTF8, 0, wide, -1, NULL, 0, NULL, NULL);
  if (size <= 0) {
    return NULL;
  }
  char* utf8 = malloc((size_t)size);
  if (utf8 != NULL && WideCharToMultiByte(CP_UTF8, 0, wide, -1, utf8, size, NULL, NULL) <= 0) {
    free(utf8);
    utf8 = NULL;
  }
  return utf8;
}

char** platform_utf8_arguments(int argument_count, wchar_t* wide_arguments[]) {
  char** arguments = calloc((size_t)argument_count + 1, sizeof *arguments);
  if (arguments == NULL) {
    return NULL;
  }
  for (int index = 0; index < argument_count; index++) {
    arguments[index] = utf16_to_utf8(wide_arguments[index]);
    if (arguments[index] == NULL) {
      platform_free_arguments(index, arguments);
      return NULL;
    }
  }
  return arguments;
}

void platform_free_arguments(int argument_count, char* arguments[]) {
  for (int index = 0; index < argument_count; index++) {
    free(arguments[index]);
  }
  free(arguments);
}

#else

void platform_use_binary_streams(void) {}

#endif
