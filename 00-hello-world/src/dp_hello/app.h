/* The program flow: scan, validate, then write exactly one kind of output (FR-11). */
#ifndef DP_HELLO_APP_H
#define DP_HELLO_APP_H

#include <stddef.h>

/* Exit statuses (SRS section 3.7, CONV-18). */
enum {
  EXIT_STATUS_SUCCESS = 0,
  EXIT_STATUS_FAILURE = 1, /* stdout unwritable (CONV-25 leaves it unspecified), no memory */
  EXIT_STATUS_USAGE = 2
};

/* Runs dp-hello on UTF-8 `arguments` (without the program name); returns the exit status. */
int app_run(size_t argument_count, const char* const arguments[]);

#endif
