/*
 * dp-hello entry point. POSIX passes UTF-8 bytes to main; on Windows (MinGW-w64, linked with
 * -municode) wmain receives the UTF-16 command line, which is converted to UTF-8 first.
 */
#include <stddef.h>

#include "app.h"
#include "platform.h"

#ifdef _WIN32
int wmain(int argc, wchar_t* argv[]) {
  platform_use_binary_streams();
  char** arguments = platform_utf8_arguments(argc, argv);
  if (arguments == NULL) {
    return EXIT_STATUS_FAILURE;
  }
  int status = app_run(argc > 0 ? (size_t)argc - 1 : 0, (const char* const*)arguments + 1);
  platform_free_arguments(argc, arguments);
  return status;
}
#else
int main(int argc, char* argv[]) {
  platform_use_binary_streams();
  return app_run(argc > 0 ? (size_t)argc - 1 : 0, (const char* const*)argv + 1);
}
#endif
