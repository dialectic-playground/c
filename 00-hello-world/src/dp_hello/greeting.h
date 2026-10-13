/* Greeting construction (NFR-05): a pure function of NAME. */
#ifndef DP_HELLO_GREETING_H
#define DP_HELLO_GREETING_H

#include <stddef.h>

/*
 * Formats the greeting line "Hello, " NAME "!" LF like snprintf: writes at most `capacity`
 * bytes including the terminating NUL (nothing when `capacity` is 0) and returns the line's
 * length in bytes, or a negative value if it cannot be represented. NAME is copied verbatim.
 */
int greeting_format(char* buffer, size_t capacity, const char* name);

#endif
