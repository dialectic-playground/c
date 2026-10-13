# Master Test Plan (MTP) — Standard Terminal Hello World

| Field | Value |
| :-- | :-- |
| Project | `00-hello-world` |
| Tools | dp-hello |
| Implementation language | C17 (GCC, Apple Clang, MinGW-w64 GCC) |
| Author | Brandon Perkins |
| Version / date | 1.0 — 2026-10-13 |
| Standard | Structure adapted from IEEE 829 / ISO/IEC/IEEE 29119-3 |

## 1. Introduction and scope

### 1.1 Objectives
"Tested enough" means: every acceptance criterion in SRS §4 is exercised by a unit test or a
black-box test, every CONV-13 diagnostic is checked byte-for-byte, the COUNT boundaries
0/1/1000/1001 are covered, and the public and hidden suites pass on Linux, macOS, Windows,
and in the container.

### 1.2 In scope / out of scope
| In scope | Out of scope |
| :-- | :-- |
| Every FR/NFR in the SRS and the applicable CONV rules, on Linux, macOS, and Windows | Terminal rendering of non-ASCII names (a console concern) |
| The build contract targets (`make all … distclean`, `roundtrip`) | Performance on hardware other than CI runners |
| Windows UTF-16 → UTF-8 arguments and binary-mode output | Input, file, and path handling (the tool has none) |

## 2. Test items

| Item | Version / location |
| :-- | :-- |
| dp-hello | `00-hello-world/src/dp_hello/` on branch `feat/00-hello-world` |
| Unit tests | `00-hello-world/src/tests/` (33 tests) |
| Public black-box suite | `tests/public/` (10 tests) |
| Hidden black-box suite | `tests/private/` (10 tests, run by CI) |

## 3. Test levels and approach

| Level | Purpose | Technique(s) | Tooling | Owner |
| :-- | :-- | :-- | :-- | :-- |
| Unit | Scanner, validation, greeting, and diagnostics in isolation | Equivalence partitioning, boundary-value analysis, error guessing | Plain C checks with the runner in `src/tests/unit_test.c` (`make test-unit`) | Developer |
| Integration | `app_run` composing the units, platform layer | Black-box through the executable | `make test` → `run-tests.py` | Developer |
| System (black-box) | The specified CLI behavior, byte-exact | Specification-based | `make test` → `run-tests.py` | Developer |
| Acceptance | Hidden suite passes on all OSes | — | CI `make check` (`test-hidden`) | CI |
| Static analysis | Warnings, style | `-Wall -Wextra -Wpedantic -Wshadow -Wstrict-prototypes -Wconversion -Werror`, `clang-format --dry-run --Werror` | `make lint` | Developer / CI |

## 4. Test design

### 4.1 Equivalence partitions and boundaries

| Input | Valid partitions | Invalid partitions | Boundaries |
| :-- | :-- | :-- | :-- |
| NAME | Non-empty: ASCII, spaces, punctuation, UTF-8, leading `-` | Empty | `""` vs one byte |
| COUNT | ASCII digits, value 1 … 1000, leading zeros | Empty, sign, space, non-digit, non-ASCII digit, 0, > 1000, overflow-sized | 0, 1, 1000, 1001; `000`, `0001000` |
| Option form | `-nX`, `-n X`, `--name=X`, `--name X`, bundles `-c2` | Unknown short/long, abbreviation, missing value, flag with `=` | Last token without its value |
| Operands | None | Any word, `-`, anything after `--` | `--` alone (valid) |
| Scan order | `--help`/`--version` first | Error before them | Error vs. help in either order |

### 4.2 Additional test cases (beyond the supplied suite)

| ID | Requirement(s) | Preconditions | Steps / command | Expected result | Level |
| :-- | :-- | :-- | :-- | :-- | :-- |
| T-01 | FR-04, FR-05 | — | `test_count_boundaries` | 0 and 1001 rejected; 1 and 1000 accepted | unit |
| T-02 | FR-04 | — | `test_count_leading_zeros` | `007` → 7, `000` rejected | unit |
| T-03 | FR-05 | — | `test_count_non_digits` | `+5`, ` 5`, `1e3`, Arabic-Indic digit rejected | unit |
| T-04 | FR-03 | — | `test_empty_name_rejected` | `invalid argument '' for '--name'` / `'-n'` | unit |
| T-05 | FR-06 | — | `test_only_the_winning_value_is_validated` | `-n '' -n B -c 0 -c 2` greets B twice | unit |
| T-06 | FR-02 | — | `test_name_may_start_with_dash` | `--name -x`, `-n-x`, `-n --help` take the value | unit |
| T-07 | CONV-12 | — | `test_help_and_version_stop_the_scan`, `test_earlier_error_beats_help` | `--help extra -x` → help; `-x --help` → error | unit |
| T-08 | FR-10, CONV-08 | — | `test_unknown_long_option` | `--nam`, `--names=x`, `--=x` unrecognized, quoted whole | unit |
| T-09 | FR-10 | — | `test_missing_values`, `test_flags_reject_values` | Exact CONV-13 texts | unit |
| T-10 | FR-09, CONV-09 | — | `test_operands_are_rejected` | `-- -n`, `-`, `-- --` → extra operand | unit |
| T-11 | CONV-05 | — | `test_unknown_short_option`, `test_utf8_name_is_byte_exact` | `-ë` quoted as one character; `Zoë` bytes preserved | unit |
| T-12 | CONV-25 | POSIX shell | `dp-hello -c 1000 \| head -1`; `dp-hello -c 1000 > /dev/full` | One line, no diagnostic; status 1 on a full device | system |
| T-13 | NFR-01, CONV-05 | Windows CI | Public and hidden suites (UTF-8 names) on windows-latest | All pass, LF only | system |

### 4.3 Negative and robustness testing
All invalid option forms, values, and operands are unit-tested (T-03 … T-10) with exact
diagnostics. Robustness (NFR-03, CONV-24): the unit tests and the public suite were also run
in the container with `CFLAGS="-O1 -g -fsanitize=address,undefined -fno-sanitize-recover"`
(`make test CFLAGS=…`), plus edge invocations (`-`, `--`, `-n`, `--name=`, a 20-digit count,
`-` followed by a lone `0xFF` byte): no sanitizer reports, no crashes. Input-file cases do not
apply (no input is read).

### 4.4 Performance and resource testing
NFR-02: `/usr/bin/time -f "%e s %M KiB" build/bin/dp-hello -c 1000 -n "Ada Lovelace"` reports
0.00 s and about 1.4 MiB peak RSS on the Linux development host, well under 100 ms and
32 MiB. The design bounds memory to one line buffer, independent of COUNT.

## 5. Test environments

| Environment | OS / version | Toolchain | How provisioned |
| :-- | :-- | :-- | :-- |
| Local | Fedora Linux 44 | GCC 16 | Host packages (`DP_RUNTIME=local`) |
| Container | Debian trixie (`gcc:16-trixie`) | GCC 16, clang-format 19, Python 3.13 | `Containerfile` (podman) |
| CI | ubuntu-latest, macos-latest, windows-latest | GCC, Apple Clang, MinGW-w64 GCC; clang-format 19.1.7 (PyPI) | `.github/workflows/ci.yml` (`native` job) |

## 6. Entry and exit criteria

- **Entry:** HLD and LLD complete; `make all` succeeds; `make lint` is clean.
- **Exit:** all unit tests, the public and hidden suites pass in the container and on all three
  OSes; `make roundtrip` passes natively; no open severity 1–2 defects; every RTM row Verified.
- **Suspension / resumption:** suspend on a build or toolchain failure on any platform; resume
  once the toolchain step is fixed and the full CI matrix is re-run.

## 7. Defect management
Defects are GitHub issues in `dialectic-playground/c` with the failing command, platform, and
expected vs. actual bytes. A fix is re-tested on the full CI matrix, and every fixed defect
gets a unit test that fails without the fix.

| Severity | Definition |
| :-- | :-- |
| 1 — Critical | Crash, or wrong output on a mainline path |
| 2 — Major | Wrong diagnostic text or exit status; platform-specific byte difference |
| 3 — Minor | Style, documentation, or lint findings with no behavioral effect |

## 8. Schedule and deliverables

| Milestone | Deliverable | Target date |
| :-- | :-- | :-- |
| Design complete | `hld.md`, `lld.md` | 2026-10-13 |
| Implementation complete | `src/`, unit tests, `Makefile` hooks | 2026-10-13 |
| Verification complete | RTM all Verified, CI green | 2026-10-13 |

## 9. Risks and contingencies

| Risk | Mitigation |
| :-- | :-- |
| Windows-only failure (CRLF, ANSI-codepage arguments) | Binary streams and `wmain`; CI on Windows for every push |
| clang-format version drift between platforms | clang-format 19.1.7 on every platform (container package, PyPI natively); `make toolchain` fails on another release line |
| A compiler warns where another does not (GCC vs Clang vs MinGW) | `-Werror` lint runs on every platform in CI |

## 10. Test summary (complete at the end)

| Metric | Value |
| :-- | :-- |
| Unit tests (pass / total) | 33 / 33 |
| Coverage (line / branch, unit tests, gcov) | `cli.c` 100 % / 100 %, `validation.c` 100 % / 100 %, `greeting.c` 100 %, `usage_error.c` 92 % / 100 %; `app.c` and `output.c` are exercised by the black-box suites |
| Public black-box (pass / 10) | 10 / 10 (container, Linux, macOS, Windows) |
| Hidden black-box (pass / 10, from CI) | 10 / 10 |
| Open defects by severity | 0 / 0 / 0 |
