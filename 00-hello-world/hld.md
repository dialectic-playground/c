# High-Level Design (HLD) — Standard Terminal Hello World

| Field | Value |
| :-- | :-- |
| Project | `00-hello-world` |
| Tools | dp-hello |
| Implementation language | C17 (GCC, Apple Clang, MinGW-w64 GCC) |
| Author | Brandon Perkins |
| Version / date | 1.0 — 2026-10-13 |
| Status | Approved |
| Inputs | `specs/URD.md`, `specs/SRS.md`, `specs/user-manual.md`, `shared/CLI-CONVENTIONS.md` |

## 1. Introduction

### 1.1 Scope
`dp-hello` writes `Hello, ` + NAME + `!` + LF to standard output COUNT times (NAME defaults to
`World`, COUNT to 1, range 1–1000), prints help and version text, and reports usage errors
with the exact CONV-13/14 diagnostics and exit status 2. As the SRS scope states, it reads no
input, takes no operands, and touches no files; it also reads no environment variables and
starts no processes (NFR-07).

### 1.2 Definitions and acronyms
| Term | Meaning |
| :-- | :-- |
| Scan | The left-to-right pass over the arguments (CONV-12) |
| Winning value | The last occurrence of `--name` or `--count` (CONV-11) |
| Usage error | Any CONV-13 condition; exit status 2 |
| MinGW-w64 | The GCC toolchain for Windows used by native CI |

### 1.3 References
- SRS §3 (interfaces), §4 (FR-01 … FR-11), §5 (NFR-01 … NFR-07), §6 (diagnostics)
- CLI-CONV §2 (CONV-03, CONV-05), §3 (CONV-07 … CONV-12), §4 (CONV-13 … CONV-15), §8

## 2. Requirements summary

| Area | SRS requirements | Architectural impact |
| :-- | :-- | :-- |
| Option scanning | FR-02, FR-06, FR-07, FR-08, FR-09, FR-10, CONV-07 … CONV-12 | A hand-written scanner (`cli`) that stops at `--help`, `--version`, or the first syntax error |
| Value validation | FR-03, FR-04, FR-05, FR-06 | A separate `validation` unit that checks only the winning values, after the scan |
| Greeting | FR-01, FR-02, FR-04, NFR-05 | A pure formatting function (`greeting`) |
| Diagnostics | FR-03, FR-05, FR-09, FR-10, FR-11, CONV-13 … CONV-15 | One error type and one formatter (`usage_error`) |
| Output and exit status | FR-07, FR-08, FR-11, §3.7, CONV-25 | One unit (`output`) owns every write; `app` maps outcomes to exit statuses |
| Portability | NFR-01, CONV-03, CONV-05 | A `platform` unit isolates Windows (binary streams, UTF-16 → UTF-8 arguments) |

## 3. Architectural overview

### 3.1 Context diagram

```text
   argv (UTF-8 bytes; UTF-16 on Windows) ──▶ ┌──────────┐ ──▶ stdout (greetings, help, version)
                                              │ dp-hello │ ──▶ stderr (usage diagnostics)
                                              └──────────┘ ──▶ exit status 0 / 2
   no stdin, files, environment, or child processes (NFR-07, CONV-26)
```

### 3.2 Component (module) decomposition

| Component | Responsibility | Depends on |
| :-- | :-- | :-- |
| `main` | Entry point; obtains UTF-8 arguments and prepares the streams | `platform`, `app` |
| `platform` | Windows binary mode and UTF-16 → UTF-8 conversion; no-op on POSIX | Win32 API, CRT |
| `app` | Runs scan → validate → output and returns the exit status | `cli`, `validation`, `output` |
| `cli` | Turns arguments into raw winning values, a help/version action, or a usage error | `usage_error` |
| `validation` | Checks NAME and COUNT, applies defaults, produces a `GreetingRequest` | `cli`, `usage_error` |
| `greeting` | Formats one greeting line (pure) | C library |
| `usage_error` | The usage-error type and the exact diagnostic text | `program` |
| `output` | Help and version texts; writes greetings and diagnostics; reports write failures | `greeting`, `usage_error` |
| `program` | Canonical name and version constants | — |

### 3.3 Data flow
`main` converts the command line to UTF-8 (Windows) and calls `app_run` with the arguments
after the program name. `cli_parse` scans them and returns a `ParsedArguments`. For help or
version, `output` writes the fixed text. For a usage error, `output` writes the diagnostic to
stderr and `app` returns 2. Otherwise `validate_arguments` produces a `GreetingRequest` or a
usage error; `output_greetings` formats the line once with `greeting_format` and writes it
COUNT times. `app` returns 0, or 1 if stdout could not be written.

## 4. Key design decisions

| # | Decision | Alternatives considered | Rationale |
| :-- | :-- | :-- | :-- |
| D1 | Validate values after the scan | Validate each occurrence while scanning | FR-06: only the winning value is validated; `-c 0 -c 2` is valid |
| D2 | The scan stops at the first `--help`, `--version`, or syntax error, and treats an operand as an immediate error | Collect all operands, then decide | CONV-12 scan order; dp-hello accepts no operands, so the first one is already the surplus |
| D3 | Errors are values (`UsageError`), formatted in one function | `fprintf` at each detection site | One mapping from error to text and status (§6); unit-testable without capturing stderr |
| D4 | `greeting_format` and `usage_error_format` follow `snprintf` semantics | Return heap strings | Pure, allocation-free functions; the caller sizes the buffer |
| D5 | Format the greeting once, write it COUNT times | Format per line; build all lines in memory | O(len(NAME)) memory regardless of COUNT |
| D6 | No getopt; a hand-written scanner | POSIX `getopt_long` | Not available on MinGW/MSVC in the same form, permutes arguments, and its messages use `argv[0]` (CONV-01) |
| D7 | Windows enters through `wmain` (`-municode`) | `GetCommandLineW` + `CommandLineToArgvW` | The CRT's own command-line parsing, no extra library; one conversion function |

## 5. Cross-platform strategy
All OS-specific code is in `platform.c` (and the `main`/`wmain` choice in `main.c`), behind
`#ifdef _WIN32`. On Windows, `platform_use_binary_streams` puts stdout and stderr in
`_O_BINARY` mode, so every LF the tool writes stays a single `0x0A` (CONV-03), and
`platform_utf8_arguments` converts the UTF-16 arguments to UTF-8 with
`WideCharToMultiByte(CP_UTF8, …)` (CONV-05), so `Zoë` is the same bytes as on Linux and macOS.
On POSIX, arguments are already bytes and are used as UTF-8 unchanged. All text is plain
ASCII literals plus the user's bytes; the program never calls `setlocale`, so formatting is
locale-independent (CONV-06). No paths are used (CONV-19 … CONV-21 do not apply).

## 6. Error handling and diagnostics
`cli` and `validation` report problems as a `UsageError` (kind, subject bytes, option as
written). Exactly one function, `usage_error_format`, maps each kind to its CONV-13 message
and appends the CONV-14 line; exactly one place, `app_run`, maps outcomes to exit statuses
(0 success, 2 usage error, 1 when stdout cannot be written). Usage errors are always detected
before anything is written to stdout (FR-11). There are no other error paths: no input, no
files, and allocation of at most two small buffers.

## 7. Non-functional design

| Quality attribute | SRS reference | How the design achieves it |
| :-- | :-- | :-- |
| Performance / memory | NFR-02 | Native executable; one pass over the arguments; one line buffer; stdio buffering for up to 1000 lines |
| Portability | NFR-01 | `platform` unit; LF-only literals; binary streams on Windows; C17 standard library only |
| Robustness | NFR-03 | Bounds-checked scanning, saturating COUNT parser, every return value checked; no aborts |
| Security / input validation | NFR-07, FR-03, FR-05 | Reads only argv; values are copied, never interpreted as format strings |
| Maintainability | NFR-04 | Separate `cli`, `validation`, `greeting`, and `output` units (one responsibility each) |
| Testability | NFR-05 | `greeting_format` and `usage_error_format` are pure and unit-tested |

## 8. Risks and open issues

| Risk / issue | Likelihood | Impact | Mitigation |
| :-- | :-- | :-- | :-- |
| Windows CRT translates LF to CRLF | M | H | `_setmode(_O_BINARY)` on both streams; public tests on Windows CI |
| Non-ASCII arguments mangled by the ANSI code page on Windows | M | H | `wmain` + UTF-8 conversion |
| clang-format versions differ between container and runners | M | L | clang-format pinned to 19.1.7 everywhere (Debian package in the container, PyPI natively); `make toolchain` enforces release line 19 |
| SIGPIPE terminates the process on a closed pipe | L | L | Allowed by CONV-25 (status unspecified) |

## 9. Approval

| Role | Name | Date | Decision |
| :-- | :-- | :-- | :-- |
| Author | Brandon Perkins | 2026-10-13 | Submitted |
| Reviewer | CI (container, Linux, macOS, Windows) | 2026-10-13 | Approved when all checks pass |
