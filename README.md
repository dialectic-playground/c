<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->

**Table of Contents** _generated with [DocToc](https://github.com/thlorenz/doctoc)_

- [c](#c)
  - [C at a glance](#c-at-a-glance)
  - [Toolchain](#toolchain)
  - [Layout](#layout)
  - [Development](#development)
  - [Implementing a project](#implementing-a-project)
  - [Contributing](#contributing)
  - [License](#license)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="https://raw.githubusercontent.com/dialectic-playground/.github/main/assets/c/dist/logo-dark.svg">
    <img alt="dialectic playground / c" src="https://raw.githubusercontent.com/dialectic-playground/.github/main/assets/c/dist/logo.svg" height="96">
  </picture>
</p>

# c

C implementation of the Dialectic Playground SDLC curriculum.

📖 **Documentation:** <https://dialectic-playground.github.io/docs/reference/languages/c/>

This repository implements the eleven projects of the
[SDLC curriculum](https://github.com/dialectic-playground/sdlc-curriculum) in C. Each project is
specified to the byte by the curriculum and verified by its black-box tests on Linux, macOS,
and Windows.

**Status:** Implemented projects: 00-hello-world.

## C at a glance

| Aspect                  | C                                                        |
| :---------------------- | :------------------------------------------------------- |
| File extension          | `.c`                                                     |
| CLI classification      | Included                                                 |
| Image category          | Native Toolchain                                         |
| Access to OS primitives | Absolute & Direct (POSIX system calls)                   |
| Entry-point mechanism   | Kernel drops argc and argv pointers directly into main() |

## Toolchain

|                      |                                                                      |
| :------------------- | :------------------------------------------------------------------- |
| Release line         | C17 (GCC 16 in the container)                                        |
| Build / package tool | make                                                                 |
| Container image      | `gcc:16-trixie`, pinned by digest ([`Containerfile`](Containerfile)) |
| Native CI            | GCC on Linux, Apple Clang on macOS, MinGW-w64 GCC on Windows         |

| Unit tests | Plain C checks with the repository's own small runner (`src/tests/unit_test.[ch]`) |
| Lint | Warning-free build with `-std=c17 -Wall -Wextra -Wpedantic -Wshadow -Wstrict-prototypes -Wconversion -Werror`, plus `clang-format --dry-run --Werror` |
| Format | `clang-format -i` (release line 19), style in [`.clang-format`](.clang-format) (shared by every project) |
| Dependencies | None (no package manager; Dependabot has no C ecosystem) |

`make toolchain` verifies that the container and native toolchains are on this release line,
and that `clang-format` is on release line 19 everywhere: the container image adds Debian
trixie's `clang-format` (19.1.7), and native CI installs the same version from PyPI
(`pip install clang-format==19.1.7`) on all three OSes. Locally, `pip install
clang-format==19.1.7` gives the same formatter. The style ([`.clang-format`](.clang-format):
Google, 100 columns) is shared with the C++ and Objective-C repositories.

Each project builds into its own `build/` directory (`make clean` removes it). `make all`
leaves the native executable in `build/bin/` (`dp-hello.exe` on Windows), and `make install`
copies it to `$(BINDIR)`. On Windows, MinGW-w64 builds enter through `wmain` (`-municode`),
convert the UTF-16 command line to UTF-8, and switch stdout and stderr to binary mode, so
arguments and output bytes match Linux and macOS exactly.

## Layout

| Path                                  | Contents                                                                                                          |
| :------------------------------------ | :---------------------------------------------------------------------------------------------------------------- |
| `00-hello-world/` … `10-sys-monitor/` | One workspace per curriculum project: `Makefile` (with `DP_*` hooks), HLD, LLD, RTM, master test plan, and `src/` |
| `sdlc-curriculum/`                    | The curriculum, as a git submodule pinned to a release tag                                                        |
| `Makefile`                            | `make check` (repository, toolchain, implemented projects), `toolchain`, `projects`, `roundtrip`                  |
| `Containerfile`, `.devcontainer/`     | The toolchain image and the matching dev container                                                                |
| `tools/dp-run`, `tools/container.mk`  | Shared container tooling (copies of the canonical files in `dialectic-playground/.github`)                        |
| `.github/workflows/ci.yml`            | CI: container and native (Linux, macOS, Windows) `make` targets, shared-tooling check, DCO                        |

## Development

Container-first: `make` targets and `tools/dp-run <command>` run in the toolchain container
via Podman, then Docker, then the local system. `DP_RUNTIME=podman|docker|local` overrides
the choice.

```sh
git clone --recurse-submodules https://github.com/dialectic-playground/c.git
cd c
make check                    # repository, toolchain, and every implemented project
DP_RUNTIME=local make check   # the same with your native toolchain
make -C 00-hello-world help   # a project's configuration and hooks
```

## Implementing a project

1. Complete the project's `hld.md` and `lld.md` before writing code.
2. Write the implementation and its unit tests under the project's `src/`, add the
   repository's make configuration, and fill in `BIN_DIR` and the `DP_*` hooks in
   the project's `Makefile`.
3. Pass `make -C <project> test` (container) and `DP_RUNTIME=local make -C <project> test`.
4. Add the project directory to `IMPLEMENTED` in the root `Makefile`. CI then builds, lints,
   and tests it in the container and natively on all three operating systems.
5. Keep `rtm.md` and `master-test-plan.md` current.

Specifications: [curriculum reference](https://dialectic-playground.github.io/docs/reference/curriculum/). Language setup:
[Set up a language repository](https://dialectic-playground.github.io/docs/how-to/set-up-a-language-repository/).

## Contributing

See the [organization contributing guide](https://github.com/dialectic-playground/.github/blob/main/CONTRIBUTING.md).
Pull requests are squash-merged, titles follow Conventional Commits, and every commit needs a
DCO sign-off.

## License

[MIT](LICENSE)
