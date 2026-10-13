# Requirements Traceability Matrix (RTM) — Standard Terminal Hello World

| Field | Value |
| :-- | :-- |
| Project | `00-hello-world` |
| Tools | dp-hello |
| Author | Brandon Perkins |
| Version / date | 1.0 — 2026-10-13 |

Code paths are relative to `src/dp_hello/`; unit tests live in `src/tests/`. Public black-box
tests are P01 … P10. The hidden suite runs in CI on Linux, macOS, and Windows and is not cited.

## 1. User requirements to system requirements

| User requirement (URD) | Derived system requirements (SRS) |
| :-- | :-- |
| UR-01 | FR-01 |
| UR-02 | FR-02 |
| UR-03 | FR-04 |
| UR-04 | FR-07 |
| UR-05 | FR-08 |
| UR-06 | FR-03, FR-05, FR-09, FR-10, FR-11 |
| UR-07 | FR-01, SRS §3.7 |
| UR-08 | NFR-01, CONV-03 |
| UR-09 | FR-02, CONV-05 |
| UR-10 | FR-06, FR-10, CONV-07 … CONV-15 |
| UR-11 | NFR-02 |
| UR-12 | NFR-06 |

## 2. Forward traceability

| Req. ID | Summary | Design (HLD/LLD) | Code (file::function) | Verification (unit / black-box) | Status |
| :-- | :-- | :-- | :-- | :-- | :-- |
| UR-01 | Classic greeting | HLD §3.3 | see FR-01 | P01 | Verified |
| UR-02 | Choose the name | HLD §3.2 | see FR-02 | P02, P03, P04 | Verified |
| UR-03 | Repeat the greeting | HLD §3.2 | see FR-04 | P05, P06 | Verified |
| UR-04 | Help screen | HLD §3.2 | see FR-07 | P07 | Verified |
| UR-05 | Version query | HLD §3.2 | see FR-08 | P08 | Verified |
| UR-06 | Clear usage errors, distinct status | HLD §6 | see FR-03, FR-05, FR-09 … FR-11 | P09, P10 | Verified |
| UR-07 | Exit status 0 on success | HLD §6 | `app.c::app_run` | P01 … P08 | Verified |
| UR-08 | Identical bytes on 3 OSes | HLD §5 | see NFR-01 | P01 … P10 on 3 OSes (CI) | Verified |
| UR-09 | Spaces and non-ASCII names | HLD §5 | see FR-02, CONV-05 | `test_utf8_name_is_byte_exact`, `test_name_value_forms` | Verified |
| UR-10 | Common CLI conventions | HLD §2 | see CONV rows | `test_cli.c` (all) | Verified |
| UR-11 | Instant start and finish | HLD §7 | see NFR-02 | MTP §4.4 | Verified |
| UR-12 | Standard install/uninstall | HLD §2 | see NFR-06 | `make roundtrip` (CI) | Verified |
| FR-01 | Default greeting | HLD §3.3, LLD §3.3, §3.4, §7 | `validation.c::validate_arguments`, `greeting.c::greeting_format`, `output.c::output_greetings` | `test_defaults`, `test_default_name`, P01 | Verified |
| FR-02 | Named greeting, all spellings, verbatim | LLD §3.1, §3.4 | `cli.c::scan_long_option`, `cli.c::scan_short_option`, `greeting.c::greeting_format` | `test_name_value_forms`, `test_name_may_start_with_dash`, `test_name_is_copied_verbatim`, P02, P03, P04, P06 | Verified |
| FR-03 | Empty name rejected | LLD §3.3 | `validation.c::validate_arguments` | `test_empty_name_rejected`, `test_empty_values_are_scanned` | Verified |
| FR-04 | Repetition 1 … 1000, leading zeros | LLD §3.2, §3.6 | `validation.c::parse_count`, `output.c::output_greetings` | `test_count_boundaries`, `test_count_leading_zeros`, `test_valid_values`, P05, P06 | Verified |
| FR-05 | Invalid count rejected | LLD §3.2, §3.3 | `validation.c::parse_count`, `validation.c::validate_arguments` | `test_count_non_digits`, `test_invalid_count_rejected` | Verified |
| FR-06 | Last occurrence wins; validate the winner only | HLD D1, LLD §3.1, §3.3 | `cli.c::store_value`, `validation.c::validate_arguments` | `test_last_occurrence_wins`, `test_only_the_winning_value_is_validated` | Verified |
| FR-07 | Help text, scan order | LLD §3.1, §3.6 | `cli.c::scan_long_option`, `output.c::HELP_TEXT`, `app.c::app_run` | `test_help_and_version_stop_the_scan`, `test_help_text_layout`, P07 | Verified |
| FR-08 | Version line | LLD §3.6 | `output.c::VERSION_TEXT`, `app.c::app_run` | `test_version_text`, P08 | Verified |
| FR-09 | Operands rejected | LLD §3.1 | `cli.c::cli_parse` | `test_operands_are_rejected`, P10 | Verified |
| FR-10 | Unknown/malformed options, no abbreviations | LLD §3.1 | `cli.c::scan_long_option`, `cli.c::scan_short_option` | `test_unknown_short_option`, `test_unknown_long_option`, `test_missing_values`, `test_flags_reject_values`, P09 | Verified |
| FR-11 | Nothing on stdout on error | HLD §6, LLD §3.7 | `app.c::app_run` | P09, P10 (empty stdout) | Verified |
| NFR-01 | Byte-identical, LF only | HLD §5, LLD §5 | `platform.c::platform_use_binary_streams`, `platform.c::platform_utf8_arguments` | P01 … P10 on Linux, macOS, Windows (CI) | Verified |
| NFR-02 | < 100 ms, < 32 MiB | HLD §7 | `output.c::output_greetings` | MTP §4.4 measurement | Verified |
| NFR-03 | No crash for any arguments | HLD §7, LLD §3.1, §3.2 | `cli.c::cli_parse`, `validation.c::parse_count` | `test_count_boundaries`, `test_missing_values`, MTP §4.3 (sanitizers) | Verified |
| NFR-04 | Separate units | HLD §3.2 | `cli.c`, `validation.c`, `greeting.c`, `output.c` | Design review (LLD §8) | Verified |
| NFR-05 | Pure, unit-tested greeting | LLD §3.4 | `greeting.c::greeting_format` | `test_is_pure`, `test_default_name`, `test_length_query_and_truncation` | Verified |
| NFR-06 | Every BASE-MAKEFILE target works | HLD §2 | `Makefile` (DP_* hooks) | `make check`, `make roundtrip` (CI, 3 OSes + container) | Verified |
| NFR-07 | No files, environment, subprocesses | HLD §1.1 | whole program (no `fopen`, `getenv`, `system`) | Inspection | Verified |
| CONV-01 | Canonical name, never argv[0] | LLD §2.3 | `program.h::PROGRAM_NAME`, `usage_error.c::usage_error_format` | `test_every_message_format`, P09, P10 | Verified |
| CONV-02 | Version format | LLD §3.6 | `output.c::VERSION_TEXT` | `test_version_text`, P08 | Verified |
| CONV-03 | LF only, binary mode on Windows | HLD §5 | `platform.c::platform_use_binary_streams` | `test_help_text_layout`, P01 … P10 on Windows | Verified |
| CONV-05 | Arguments as UTF-8; UTF-16 → UTF-8 on Windows | HLD §5, LLD §3.8 | `main.c::wmain`, `platform.c::platform_utf8_arguments` | `test_utf8_name_is_byte_exact`, `test_unknown_short_option`, P02 … P06 on Windows | Verified |
| CONV-06 | Locale-independent, plain ASCII numbers | HLD §5 | no `setlocale`; `validation.c::parse_count` | `test_count_non_digits` | Verified |
| CONV-07 | Short options, attached or separate values | LLD §3.1 | `cli.c::scan_short_option` | `test_bundled_and_separate_values`, P03, P06 | Verified |
| CONV-08 | Long options, `=` or separate, no abbreviation | LLD §3.1 | `cli.c::scan_long_option`, `cli.c::long_name_is` | `test_name_value_forms`, `test_unknown_long_option`, P02, P04, P05 | Verified |
| CONV-09 | `--` terminator | LLD §3.1 | `cli.c::cli_parse` | `test_terminator_alone_is_accepted`, `test_operands_are_rejected` | Verified |
| CONV-10 | Operands (incl. `-`) | LLD §3.1 | `cli.c::cli_parse` | `test_operands_are_rejected`, P10 | Verified |
| CONV-11 | Last occurrence wins | LLD §3.1 | `cli.c::store_value` | `test_last_occurrence_wins` | Verified |
| CONV-12 | Scan order; errors before output | HLD D2, LLD §4 | `cli.c::cli_parse`, `app.c::app_run` | `test_help_and_version_stop_the_scan`, `test_earlier_error_beats_help` | Verified |
| CONV-13 | Exact usage messages | LLD §2.3, §3.5 | `usage_error.c::usage_error_format` | `test_every_message_format`, `test_cli.c` (all error tests), P09, P10 | Verified |
| CONV-14 | "Try" line, exit 2 | LLD §3.5, §3.7 | `usage_error.c::usage_error_format`, `app.c::report_usage_error` | `test_every_message_format`, P09, P10 | Verified |
| CONV-15 | Values quoted verbatim, `''` when empty | LLD §3.5 | `usage_error.c::usage_error_format` | `test_values_are_not_escaped`, `test_empty_name_rejected` | Verified |
| CONV-18 | Exit statuses (as overridden by SRS §3.7) | HLD §6 | `app.h` exit-status constants, `app.c::app_run` | P01 … P10 (`.exit` files) | Verified |
| CONV-22 | `--help` and `--version` accepted, no argument | LLD §3.1 | `cli.c::scan_long_option` | `test_flags_reject_values`, P07, P08 | Verified |
| CONV-23 | Help text verbatim | LLD §3.6 | `output.c::HELP_TEXT` | `test_help_text_layout`, P07 | Verified |
| CONV-24 | No crash or stack trace | HLD §7 | all units (checked returns, bounded scans) | MTP §4.3 | Verified |
| CONV-25 | EPIPE: stop promptly, no diagnostic | LLD §3.6, §5 | `output.c::output_greetings` | MTP T-12 (`dp-hello -c 1000 \| head -1`) | Verified |
| CONV-26 | stdin not read | HLD §3.1 | no stdin access anywhere | Inspection; P01 … P10 (stdin is the null device) | Verified |
| CONV-27 … CONV-30 | Harness conditions | MTP §3 | native executable in `build/bin` | P01 … P10 | Verified |

## 3. Backward traceability (test → requirement)

| Test | Type | Requirement(s) verified |
| :-- | :-- | :-- |
| P01 | black-box, public | FR-01, UR-01, UR-07 |
| P02 | black-box, public | FR-02, CONV-08 |
| P03 | black-box, public | FR-02, CONV-07 |
| P04 | black-box, public | FR-02, CONV-08 |
| P05 | black-box, public | FR-04 |
| P06 | black-box, public | FR-04, CONV-07 |
| P07 | black-box, public | FR-07, CONV-23 |
| P08 | black-box, public | FR-08, CONV-02 |
| P09 | black-box, public | FR-10, FR-11, CONV-13, CONV-14 |
| P10 | black-box, public | FR-09, FR-11, CONV-10, CONV-13 |
| `test_no_arguments_greets_with_defaults` | unit | FR-01 |
| `test_name_value_forms` | unit | FR-02, CONV-07, CONV-08 |
| `test_name_may_start_with_dash` | unit | FR-02 |
| `test_empty_values_are_scanned` | unit | FR-03 |
| `test_bundled_and_separate_values` | unit | FR-04, CONV-07, CONV-08 |
| `test_last_occurrence_wins` | unit | FR-06, CONV-11 |
| `test_help_and_version_stop_the_scan` | unit | FR-07, FR-08, CONV-12 |
| `test_earlier_error_beats_help` | unit | CONV-12 |
| `test_unknown_short_option` | unit | FR-10, CONV-05, CONV-13 |
| `test_unknown_long_option` | unit | FR-10, CONV-08, CONV-13 |
| `test_missing_values` | unit | FR-10, CONV-13 |
| `test_flags_reject_values` | unit | FR-10, CONV-22 |
| `test_operands_are_rejected` | unit | FR-09, CONV-09, CONV-10 |
| `test_terminator_alone_is_accepted` | unit | CONV-09 |
| `test_count_boundaries` | unit | FR-04, FR-05, NFR-03 |
| `test_count_leading_zeros` | unit | FR-04 |
| `test_count_non_digits` | unit | FR-05, CONV-06 |
| `test_defaults` | unit | FR-01 |
| `test_valid_values` | unit | FR-02, FR-04 |
| `test_empty_name_rejected` | unit | FR-03, CONV-15 |
| `test_invalid_count_rejected` | unit | FR-05 |
| `test_only_the_winning_value_is_validated` | unit | FR-06 |
| `test_name_is_validated_before_count` | unit | FR-03, FR-05 |
| `test_default_name` | unit | FR-01, NFR-05 |
| `test_name_is_copied_verbatim` | unit | FR-02, NFR-05 |
| `test_utf8_name_is_byte_exact` | unit | FR-02, CONV-05 |
| `test_length_query_and_truncation` | unit | NFR-05 |
| `test_is_pure` | unit | NFR-05 |
| `test_every_message_format` | unit | CONV-01, CONV-13, CONV-14 |
| `test_values_are_not_escaped` | unit | CONV-15 |
| `test_subject_length_is_respected` | unit | CONV-13 |
| `test_version_text` | unit | FR-08, CONV-02 |
| `test_help_text_layout` | unit | FR-07, CONV-03, CONV-23 |

## 4. Coverage summary

| Category | Total | Designed | Implemented | Verified |
| :-- | :-- | :-- | :-- | :-- |
| User requirements (UR) | 12 | 12 | 12 | 12 |
| Functional (FR) | 11 | 11 | 11 | 11 |
| Non-functional (NFR) | 7 | 7 | 7 | 7 |
| Conventions (CONV) | 24 applicable of 30 | 24 | 24 | 24 |

## 5. Gaps and justifications
No applicable requirement is unverified. Not applicable to dp-hello, which reads no input and
uses no paths: CONV-04 (copying input), CONV-16 and CONV-17 (file errors, several operands),
CONV-19 … CONV-21 (paths and traversal). The "missing operand" row of CONV-13 cannot occur.
CONV-27 … CONV-30 describe the test harness and are counted as four requirements above.
