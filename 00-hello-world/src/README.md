<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->

**Table of Contents** _generated with [DocToc](https://github.com/thlorenz/doctoc)_

- [Source: 00-hello-world (C17)](#source-00-hello-world-c17)
  - [Implementation contract](#implementation-contract)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

# Source: 00-hello-world (C17)

`dp-hello`, implemented in C17 with the standard library only. The design is in `../hld.md`
and `../lld.md`; the behavior is specified by
`../../sdlc-curriculum/projects/00-hello-world/specs/SRS.md`, `shared/CLI-CONVENTIONS.md`, and
the help text in `../../sdlc-curriculum/projects/00-hello-world/specs/user-manual.md`.

```text
src/
├── dp_hello/              the tool
│   ├── main.c             entry point: main (POSIX) or wmain (Windows, -municode)
│   ├── platform.[ch]      Windows only: binary-mode stdout/stderr, UTF-16 → UTF-8 arguments
│   ├── app.[ch]           program flow and exit statuses
│   ├── cli.[ch]           option scanning (CONV-07 … CONV-12), syntax errors
│   ├── validation.[ch]    NAME and COUNT validation, defaults
│   ├── greeting.[ch]      greeting construction (pure)
│   ├── usage_error.[ch]   usage-error type and exact CONV-13/14 diagnostic text
│   ├── output.[ch]        help and version text; all writes to stdout and stderr
│   └── program.h          canonical name and version
└── tests/                 unit tests: plain C checks and a small runner
    ├── unit_test.[ch]     runner (main), EXPECT / EXPECT_STRING / RUN_TEST
    ├── test_support.[ch]  argument-list and diagnostic helpers
    ├── test_cli.c         parsing
    ├── test_validation.c  validation
    ├── test_greeting.c    greeting construction
    └── test_usage_error.c diagnostics, help and version texts
```

From `..`: `make all` (build into `build/bin/`), `make test` (unit tests, then the public
black-box suite), `make lint`, `make format`, `make install`, `make clean`.

## Implementation contract

An implementation of `00-hello-world` is complete when, from its project directory:

1. `make all` builds these executables into `BIN_DIR`: **dp-hello**. Each is a native
   executable (with an `.exe` suffix on Windows) or a script that starts with a `#!` line,
   which works on every OS, including Windows (see `shared/templates/BASE-MAKEFILE`).
   `make install` puts the executable, or a launcher, into `$(BINDIR)`. Script payloads go
   under `$(LIBDIR)/<repository>/<project>/`, and `make uninstall` removes everything it
   installed.
2. Every standard target in `shared/templates/BASE-MAKEFILE` works (`all install uninstall
test check lint format clean distclean`), implemented with that language's usual tools.
3. `make test` runs the language's unit tests, then passes all 10 public black-box tests.
4. `make test-hidden` passes all 10 hidden black-box tests on Linux, macOS, and Windows.
5. The behavior matches the SRS and `shared/CLI-CONVENTIONS.md` byte-for-byte, and the help
   text matches the user manual.
6. The white-box deliverables in `../` (`hld.md`, `lld.md`, `rtm.md`,
   `master-test-plan.md`) are completed for this implementation.
7. The code follows the 20 engineering principles in the curriculum's root `README.md`.
8. The repository's `Containerfile` provides the toolchain, so every target works
   container-first (`make test`) as well as natively (`DP_RUNTIME=local make test`).
