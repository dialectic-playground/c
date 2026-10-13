/* Platform layer: the only unit with operating-system-specific code (CONV-03, CONV-05). */
#ifndef DP_HELLO_PLATFORM_H
#define DP_HELLO_PLATFORM_H

/*
 * Switches stdout and stderr to binary mode on Windows, so LF is never translated to CRLF.
 * Does nothing on POSIX systems, where streams are already byte-transparent.
 */
void platform_use_binary_streams(void);

#ifdef _WIN32
#include <wchar.h>

/*
 * Converts the UTF-16 arguments that wmain receives to a newly allocated array of
 * `argument_count` UTF-8 strings, or returns NULL if memory runs out. Release the array with
 * platform_free_arguments.
 */
char** platform_utf8_arguments(int argument_count, wchar_t* wide_arguments[]);

/* Releases an array returned by platform_utf8_arguments. */
void platform_free_arguments(int argument_count, char* arguments[]);
#endif

#endif
