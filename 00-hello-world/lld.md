# Low-Level Design (LLD) — Standard Terminal Hello World

| Field | Value |
| :-- | :-- |
| Project | `00-hello-world` |
| Tools | dp-hello |
| Implementation language | C17 (GCC, Apple Clang, MinGW-w64 GCC) |
| Author | Brandon Perkins |
| Version / date | 1.0 — 2026-10-13 |
| Status | Approved |
| Inputs | `hld.md`, `specs/SRS.md`, `shared/CLI-CONVENTIONS.md` |

## 1. Source layout

```text
src/
├── dp_hello/
│   ├── main.c            main (POSIX) / wmain (Windows): arguments → app_run
│   ├── platform.[ch]     platform_use_binary_streams, platform_utf8_arguments (Windows)
│   ├── app.[ch]          app_run, exit-status constants
│   ├── cli.[ch]          cli_parse and its scanner helpers
│   ├── validation.[ch]   parse_count, validate_arguments, defaults and limits
│   ├── greeting.[ch]     greeting_format
│   ├── usage_error.[ch]  UsageError, usage_error_make, usage_error_format
│   ├── output.[ch]       HELP_TEXT, VERSION_TEXT, output_text, output_greetings, output_usage_error
│   └── program.h         PROGRAM_NAME, PROGRAM_VERSION
└── tests/
    ├── unit_test.[ch]    runner main, TestContext, EXPECT, EXPECT_STRING, RUN_TEST
    ├── test_support.[ch] ARGS/NO_ARGS lists, parse_list, render_error
    ├── test_cli.c, test_validation.c, test_greeting.c, test_usage_error.c
```

The unit-test executable links every `dp_hello/*.c` except `main.c`.

## 2. Data types

### 2.1 `ParsedArguments` (`cli.h`)
| Field | Type | Default | Constraint / invariant | SRS |
| :-- | :-- | :-- | :-- | :-- |
| `action` | `CliAction` (`CLI_GREET`, `CLI_HELP`, `CLI_VERSION`, `CLI_USAGE_ERROR`) | `CLI_GREET` | Set once the scan stops | FR-07, FR-08, CONV-12 |
| `name` | `OptionValue {value, option}` | `{NULL, NULL}` | Last occurrence; `option` is `"-n"` or `"--name"` | FR-02, FR-06 |
| `count` | `OptionValue` | `{NULL, NULL}` | Last occurrence; `option` is `"-c"` or `"--count"` | FR-04, FR-06 |
| `error` | `UsageError` | — | Meaningful only for `CLI_USAGE_ERROR` | FR-09, FR-10 |

All pointers refer to the argument strings or string literals; nothing is allocated.

### 2.2 `GreetingRequest` (`validation.h`)
| Field | Type | Default | Constraint / invariant | SRS |
| :-- | :-- | :-- | :-- | :-- |
| `name` | `const char *` | `"World"` | Non-empty | FR-01, FR-02, FR-03 |
| `count` | `unsigned` | 1 | 1 ≤ count ≤ 1000 | FR-04, FR-05 |

### 2.3 Error type: `UsageError` (`usage_error.h`)
`{kind, subject, subject_length, option}`. `subject_length` lets an unknown short option be
quoted as a whole UTF-8 character. Every variant exits with status 2 and is followed by
`Try 'dp-hello --help' for more information.`

| Variant | Diagnostic (stderr, first line) | Exit status |
| :-- | :-- | :-- |
| `USAGE_INVALID_OPTION` | `dp-hello: invalid option -- 'x'` | 2 |
| `USAGE_UNRECOGNIZED_OPTION` | `dp-hello: unrecognized option '<token>'` | 2 |
| `USAGE_SHORT_MISSING_VALUE` | `dp-hello: option requires an argument -- 'n'` | 2 |
| `USAGE_LONG_MISSING_VALUE` | `dp-hello: option '--name' requires an argument` | 2 |
| `USAGE_LONG_UNEXPECTED_VALUE` | `dp-hello: option '--help' doesn't allow an argument` | 2 |
| `USAGE_INVALID_VALUE` | `dp-hello: invalid argument '<value>' for '<option>'` | 2 |
| `USAGE_EXTRA_OPERAND` | `dp-hello: extra operand '<operand>'` | 2 |

## 3. Module and function specifications

### 3.1 `cli_parse`
- **Signature:** `ParsedArguments cli_parse(size_t argument_count, const char *const arguments[])`
- **Responsibility:** scan the arguments (without the program name) left to right.
- **Preconditions:** `arguments[0 … argument_count-1]` are NUL-terminated UTF-8 strings.
- **Postconditions / return value:** the action plus the last `--name`/`--count` occurrences.
- **Errors:** every variant except `USAGE_INVALID_VALUE`.
- **Algorithm:** for each token: after `--` every token is an operand; `--` sets that state;
  a token starting with `--` goes to `scan_long_option`; `-` + at least one byte goes to
  `scan_short_option`; anything else (including `-`) is an operand, and the first one is
  remembered. Stop at the first option syntax error, `--help`, or `--version`. After the scan,
  if the action is still `CLI_GREET` and an operand was seen, report `USAGE_EXTRA_OPERAND`. `scan_long_option` splits at the first `=`, compares
  the name exactly with `name`, `count`, `help`, `version` (no abbreviations), takes the value
  from after `=` or the next token (which may start with `-`), and reports a flag given `=…`.
  `scan_short_option` looks only at the first letter, because both short options take a value:
  the rest of the token, or else the next token, is the value.
- **Complexity:** time O(total argument bytes), memory O(1).
- **Satisfies:** FR-02, FR-06 … FR-10, CONV-07 … CONV-13
- **Unit tests:** every test in `src/tests/test_cli.c` (listed in `rtm.md` §3)

### 3.2 `parse_count`
- **Signature:** `bool parse_count(const char *text, unsigned *count)`
- **Responsibility:** accept one or more ASCII digits with a value in 1 … 1000.
- **Postconditions:** on success `*count` holds the value; on failure it is unchanged.
- **Algorithm:** reject an empty string or any byte outside `'0'…'9'`; accumulate the value but
  stop multiplying once it exceeds 1000 (saturation, so no overflow); check the range.
- **Complexity:** O(len(text)), O(1) memory.
- **Satisfies:** FR-04, FR-05
- **Unit tests:** `test_count_boundaries`, `test_count_leading_zeros`, `test_count_non_digits`

### 3.3 `validate_arguments`
- **Signature:** `bool validate_arguments(const ParsedArguments *arguments, GreetingRequest *request, UsageError *error)`
- **Responsibility:** check the winning values and apply defaults.
- **Errors:** `USAGE_INVALID_VALUE` for an empty NAME (checked first) or an invalid COUNT, with
  the option as written.
- **Satisfies:** FR-01, FR-03, FR-05, FR-06, CONV-15
- **Unit tests:** `test_defaults`, `test_valid_values`, `test_empty_name_rejected`,
  `test_invalid_count_rejected`, `test_only_the_winning_value_is_validated`,
  `test_name_is_validated_before_count`

### 3.4 `greeting_format`
- **Signature:** `int greeting_format(char *buffer, size_t capacity, const char *name)`
- **Responsibility:** produce `Hello, ` + NAME + `!` + LF; pure (no I/O, no state).
- **Return value:** the line length in bytes, as `snprintf` (`%s` copies NAME verbatim).
- **Satisfies:** FR-01, FR-02, NFR-05, CONV-05
- **Unit tests:** `test_default_name`, `test_name_is_copied_verbatim`,
  `test_utf8_name_is_byte_exact`, `test_length_query_and_truncation`, `test_is_pure`

### 3.5 `usage_error_format`
- **Signature:** `int usage_error_format(char *buffer, size_t capacity, const UsageError *error)`
- **Responsibility:** the complete two-line diagnostic for `error` (table §2.3); pure.
- **Satisfies:** CONV-01, CONV-13, CONV-14, CONV-15
- **Unit tests:** `test_every_message_format`, `test_values_are_not_escaped`,
  `test_subject_length_is_respected`

### 3.6 `output_text`, `output_greetings`, `output_usage_error`
- **Signatures:** `bool output_text(FILE *, const char *)`,
  `bool output_greetings(FILE *, const GreetingRequest *)`,
  `void output_usage_error(FILE *, const UsageError *)`
- **Responsibility:** the only writes to the standard streams. `output_greetings` sizes and
  allocates one line buffer, formats it once, writes it `count` times with `fwrite`, stops at
  the first failed write, and flushes. Returns false on any write or flush failure.
- **Satisfies:** FR-01, FR-04, FR-07, FR-08, CONV-23, CONV-25
- **Unit tests:** `test_help_text_layout`, `test_version_text`; P01 … P10

### 3.7 `app_run`
- **Signature:** `int app_run(size_t argument_count, const char *const arguments[])`
- **Responsibility:** `cli_parse` → (`output_text` | `validate_arguments` → `output_greetings`),
  or `output_usage_error`; returns 0, 2, or 1 (stdout not writable).
- **Satisfies:** FR-07, FR-08, FR-11, SRS §3.7
- **Unit tests:** black-box P01 … P10 (it only composes unit-tested functions)

### 3.8 `platform_use_binary_streams`, `platform_utf8_arguments`
- **Responsibility:** Windows: `_setmode(_fileno(stdout|stderr), _O_BINARY)`; convert each
  `wchar_t *` argument with `WideCharToMultiByte(CP_UTF8, 0, …)` into a `calloc`'d,
  NULL-terminated array (released by `platform_free_arguments`). POSIX: no-op.
- **Satisfies:** NFR-01, CONV-03, CONV-05
- **Unit tests:** black-box P01 … P10 on Windows CI (UTF-8 names: hidden suite)

## 4. Algorithms
Only the scan is non-trivial; see §3.1. Scanning stops at the first decisive token, so
`--help extra -x` prints help (FR-07) while `-x --help` reports `-x` (CONV-12). Values are not
validated during the scan (FR-06), so `--count 0 --help` prints help. Operands are only collected
during the scan and rejected after it (CONV-12 as clarified in curriculum v1.4.0), so
`extra --help` prints help while `extra -x` reports `-x`, and `extra` alone reports the operand. An unknown short option is quoted as
its whole UTF-8 sequence (lead byte plus continuation bytes), so `-ë` gives
`invalid option -- 'ë'` rather than a broken byte.

## 5. Platform-specific details

| Concern | Linux | macOS | Windows | Abstraction |
| :-- | :-- | :-- | :-- | :-- |
| Argument encoding | bytes (UTF-8) | bytes (UTF-8) | UTF-16 via `wmain` → UTF-8 | `main.c`, `platform_utf8_arguments` |
| Newline translation | none | none | text mode would write CRLF: `_O_BINARY` | `platform_use_binary_streams` |
| Entry point / link flags | `main` | `main` | `wmain`, linked with `-municode` | `main.c`, project `Makefile` |
| Compiler | GCC | Apple Clang | MinGW-w64 GCC | `CC` default in `Makefile` |
| Broken pipe | SIGPIPE ends the process | same | `fwrite` fails, output stops, status 1 | `output_greetings` |

## 6. Resource management
No files, sockets, or processes are opened. `output_greetings` and `output_usage_error` each
allocate one buffer of the exact formatted length and free it before returning. On Windows,
the UTF-8 argument array is freed after `app_run`; on allocation failure the tool exits with
status 1 without output. Memory use is O(total argument length).

## 7. Sequence of one invocation
P01, `dp-hello`: `main` → `platform_use_binary_streams` (no-op) → `app_run(0, …)` →
`cli_parse` returns `CLI_GREET` with no values → `validate_arguments` yields
`{"World", 1}` → `output_greetings` calls `greeting_format(NULL, 0, "World")` (14), allocates
15 bytes, formats `Hello, World!\n`, writes it once, flushes → `app_run` returns 0.

## 8. Design-to-code checklist

| HLD component | LLD section | Source file(s) | Unit-test file(s) |
| :-- | :-- | :-- | :-- |
| `cli` | §3.1 | `src/dp_hello/cli.c` | `src/tests/test_cli.c` |
| `validation` | §3.2, §3.3 | `src/dp_hello/validation.c` | `src/tests/test_validation.c` |
| `greeting` | §3.4 | `src/dp_hello/greeting.c` | `src/tests/test_greeting.c` |
| `usage_error` | §2.3, §3.5 | `src/dp_hello/usage_error.c` | `src/tests/test_usage_error.c` |
| `output` | §3.6 | `src/dp_hello/output.c` | `src/tests/test_usage_error.c`, P07, P08 |
| `app` | §3.7 | `src/dp_hello/app.c` | P01 … P10 |
| `platform`, `main` | §3.8, §5 | `src/dp_hello/platform.c`, `src/dp_hello/main.c` | P01 … P10 (Windows CI) |
