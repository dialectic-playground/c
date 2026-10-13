#include "output.h"

#include <stdlib.h>
#include <string.h>

#include "greeting.h"
#include "program.h"

const char HELP_TEXT[] =
    "Usage: " PROGRAM_NAME
    " [OPTION]...\n"
    "Print a greeting to standard output.\n"
    "\n"
    "Options:\n"
    "  -n, --name=NAME     greet NAME instead of World\n"
    "  -c, --count=COUNT   print the greeting COUNT times (1-1000, default 1)\n"
    "      --help          display this help and exit\n"
    "      --version       output version information and exit\n";

const char VERSION_TEXT[] = PROGRAM_NAME " (dialectic-playground) " PROGRAM_VERSION "\n";

static bool write_bytes(FILE* stream, const char* bytes, size_t length) {
  return fwrite(bytes, 1, length, stream) == length;
}

bool output_text(FILE* stream, const char* text) {
  bool written = write_bytes(stream, text, strlen(text));
  return fflush(stream) == 0 && written;
}

bool output_greetings(FILE* stream, const GreetingRequest* request) {
  int length = greeting_format(NULL, 0, request->name);
  if (length < 0) {
    return false;
  }
  char* line = malloc((size_t)length + 1);
  if (line == NULL) {
    return false;
  }
  greeting_format(line, (size_t)length + 1, request->name);
  bool written = true;
  /* Stop at the first failure, e.g. a closed pipe (CONV-25). */
  for (unsigned repetition = 0; written && repetition < request->count; repetition++) {
    written = write_bytes(stream, line, (size_t)length);
  }
  free(line);
  return fflush(stream) == 0 && written;
}

void output_usage_error(FILE* stream, const UsageError* error) {
  int length = usage_error_format(NULL, 0, error);
  char* message = length < 0 ? NULL : malloc((size_t)length + 1);
  if (message == NULL) {
    return;
  }
  usage_error_format(message, (size_t)length + 1, error);
  /* Diagnostics are best effort: there is nowhere left to report a stderr failure. */
  (void)write_bytes(stream, message, (size_t)length);
  (void)fflush(stream);
  free(message);
}
