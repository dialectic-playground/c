#include "cli.h"

#include <string.h>

/* A value option: its short letter and both spellings, as written in diagnostics. */
typedef struct {
  char letter;
  const char* short_spelling;
  const char* long_spelling;
} ValueOption;

static const ValueOption NAME_OPTION = {'n', "-n", "--name"};
static const ValueOption COUNT_OPTION = {'c', "-c", "--count"};

/* A long option without a value and the action it selects. */
typedef struct {
  const char* spelling;
  CliAction action;
} FlagOption;

static const FlagOption FLAG_OPTIONS[] = {{"--help", CLI_HELP}, {"--version", CLI_VERSION}};

/* Scanner state: the argument list, the index of the next token, and the result so far. */
typedef struct {
  size_t count;
  const char* const* tokens;
  size_t next;
  ParsedArguments result;
} Scanner;

/* Records a usage error; the scan stops at the first one (CONV-12). */
static bool fail(Scanner* scanner, UsageErrorKind kind, const char* subject,
                 size_t subject_length) {
  UsageError error = {kind, subject, subject_length, NULL};
  scanner->result.action = CLI_USAGE_ERROR;
  scanner->result.error = error;
  return false;
}

/* Takes the next token as an option value, even if it starts with '-' (FR-02). */
static const char* take_next_token(Scanner* scanner) {
  return scanner->next < scanner->count ? scanner->tokens[scanner->next++] : NULL;
}

/* Stores the latest occurrence of a value option; the last one wins (CONV-11). */
static void store_value(Scanner* scanner, const ValueOption* option, const char* value,
                        const char* spelling) {
  OptionValue occurrence = {value, spelling};
  if (option == &NAME_OPTION) {
    scanner->result.name = occurrence;
  } else {
    scanner->result.count = occurrence;
  }
}

/* True when the long option name `name` (of `length` bytes) is exactly `spelling` minus "--". */
static bool long_name_is(const char* name, size_t length, const char* spelling) {
  return strlen(spelling + 2) == length && strncmp(name, spelling + 2, length) == 0;
}

/* Length of the UTF-8 sequence starting at `text`, so `-ë` is quoted as one character. */
static size_t utf8_character_length(const char* text) {
  unsigned char lead = (unsigned char)text[0];
  size_t expected = lead >= 0xF0 ? 4 : lead >= 0xE0 ? 3 : lead >= 0xC0 ? 2 : 1;
  size_t length = 1;
  while (length < expected && ((unsigned char)text[length] & 0xC0) == 0x80) {
    length++;
  }
  return length;
}

/* Handles a token that starts with "--" (CONV-08); returns false when the scan must stop. */
static bool scan_long_option(Scanner* scanner, const char* token) {
  const char* name = token + 2;
  const char* equals = strchr(name, '=');
  size_t length = equals != NULL ? (size_t)(equals - name) : strlen(name);

  const ValueOption* value_options[] = {&NAME_OPTION, &COUNT_OPTION};
  for (size_t index = 0; index < sizeof value_options / sizeof value_options[0]; index++) {
    const ValueOption* option = value_options[index];
    if (long_name_is(name, length, option->long_spelling)) {
      const char* value = equals != NULL ? equals + 1 : take_next_token(scanner);
      if (value == NULL) {
        return fail(scanner, USAGE_LONG_MISSING_VALUE, option->long_spelling,
                    strlen(option->long_spelling));
      }
      store_value(scanner, option, value, option->long_spelling);
      return true;
    }
  }

  for (size_t index = 0; index < sizeof FLAG_OPTIONS / sizeof FLAG_OPTIONS[0]; index++) {
    const FlagOption* flag = &FLAG_OPTIONS[index];
    if (long_name_is(name, length, flag->spelling)) {
      if (equals != NULL) {
        return fail(scanner, USAGE_LONG_UNEXPECTED_VALUE, flag->spelling, strlen(flag->spelling));
      }
      scanner->result.action = flag->action;
      return false;
    }
  }

  /* No abbreviations: anything else is unrecognized, quoted whole (CONV-13). */
  return fail(scanner, USAGE_UNRECOGNIZED_OPTION, token, strlen(token));
}

/*
 * Handles a token such as "-c2" or "-n" (CONV-07). Both short options take a value, so the
 * first letter of a bundle always consumes the rest of the token (or the next token).
 */
static bool scan_short_option(Scanner* scanner, const char* token) {
  const char* letter = token + 1;
  const ValueOption* option = *letter == NAME_OPTION.letter    ? &NAME_OPTION
                              : *letter == COUNT_OPTION.letter ? &COUNT_OPTION
                                                               : NULL;
  if (option == NULL) {
    return fail(scanner, USAGE_INVALID_OPTION, letter, utf8_character_length(letter));
  }
  const char* value = letter[1] != '\0' ? letter + 1 : take_next_token(scanner);
  if (value == NULL) {
    return fail(scanner, USAGE_SHORT_MISSING_VALUE, letter, 1);
  }
  store_value(scanner, option, value, option->short_spelling);
  return true;
}

ParsedArguments cli_parse(size_t argument_count, const char* const arguments[]) {
  Scanner scanner = {argument_count,
                     arguments,
                     0,
                     {CLI_GREET, {NULL, NULL}, {NULL, NULL}, {USAGE_EXTRA_OPERAND, NULL, 0, NULL}}};
  bool options_ended = false;
  bool keep_scanning = true;
  while (keep_scanning && scanner.next < scanner.count) {
    const char* token = take_next_token(&scanner);
    if (!options_ended && strcmp(token, "--") == 0) {
      options_ended = true; /* CONV-09: every later token is an operand. */
    } else if (!options_ended && strncmp(token, "--", 2) == 0) {
      keep_scanning = scan_long_option(&scanner, token);
    } else if (!options_ended && token[0] == '-' && token[1] != '\0') {
      keep_scanning = scan_short_option(&scanner, token);
    } else {
      /* dp-hello accepts no operands (FR-09). */
      keep_scanning = fail(&scanner, USAGE_EXTRA_OPERAND, token, strlen(token));
    }
  }
  return scanner.result;
}
