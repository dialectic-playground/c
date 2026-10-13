#include "app.h"

#include <stdio.h>

#include "cli.h"
#include "output.h"
#include "validation.h"

static int report_usage_error(const UsageError* error) {
  output_usage_error(stderr, error);
  return EXIT_STATUS_USAGE;
}

static int succeed_if(bool written) { return written ? EXIT_STATUS_SUCCESS : EXIT_STATUS_FAILURE; }

int app_run(size_t argument_count, const char* const arguments[]) {
  ParsedArguments parsed = cli_parse(argument_count, arguments);
  switch (parsed.action) {
    case CLI_HELP:
      return succeed_if(output_text(stdout, HELP_TEXT));
    case CLI_VERSION:
      return succeed_if(output_text(stdout, VERSION_TEXT));
    case CLI_USAGE_ERROR:
      return report_usage_error(&parsed.error);
    case CLI_GREET:
      break;
  }
  GreetingRequest request;
  UsageError error;
  if (!validate_arguments(&parsed, &request, &error)) {
    return report_usage_error(&error);
  }
  return succeed_if(output_greetings(stdout, &request));
}
