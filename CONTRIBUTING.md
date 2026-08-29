# Contributing to bit_set

## What a PR must satisfy before it can merge

This repository enforces its quality bar through CI rather than through review discretion. A PR is mergeable once every required check below is green.

- **Every compiler/platform leg passes.** See the table in [README.md](README.md) for the current matrix (GCC, Clang, Clang libc++, Clang-CL, MSVC, MinGW, Apple Clang). Every leg counts, including every `Development` leg (`17-SVN`, `24-SVN`, `2026-Preview`), the `libc++` legs, and the VS 2022 legs; none of them are advisory. Each ladder workflow ends in an `all` job that is red unless every one of its legs succeeded, and that gate is what branch protection requires: a leg name changes whenever a rung moves, the gate name does not.
- **`clang-tidy` is clean.** [`.clang-tidy`](.clang-tidy) sets `WarningsAsErrors: '*'`, so any finding over the public headers fails the job outright.
- **MSVC's `/analyze` is clean.** The [MSVC-Analyze workflow](.github/workflows/msvc-analyze.yml) fills the same role on the MSVC side, on all three Visual Studio rungs.
- **No sanitizer failures.** The [Sanitizers workflow](.github/workflows/sanitizers.yml) runs ASan+LSan, UBSan and the implicit-conversion sanitizer, against both libstdc++ and libc++. Leak detection is on.
- **The public headers stay self-sufficient.** Each header is compiled as its own translation unit (see `test/CMakeLists.txt`); don't rely on include order from another header.
- **Workflow files pass `actionlint`.** The [Actionlint workflow](.github/workflows/actionlint.yml) validates GitHub Actions syntax and expressions.
- **The documented consumption methods work.** The [Consumption workflow](.github/workflows/consumption.yml) builds a consumer using `find_package`, `add_subdirectory`, and `FetchContent`.
- **CodeQL analysis is clean.** The [CodeQL workflow](.github/workflows/codeql.yml) runs the C/C++ `security-extended` query suite.

Two checks run without being required:

- **Coverage is reported, not enforced.** The test suite is still being built up, so the [Coverage workflow](.github/workflows/coverage.yml) passes `fail_under_line: 0` and `fail_under_branch: 0`, and [`codecov.yml`](.github/codecov.yml) marks both Codecov statuses `informational: true`. Today's figure is roughly 85% of lines and 60% of branches. Raise the floors as coverage is built up, and make the checks required once they reach the bar.
- **`clang-format` does not run on a PR.** This repository has no `.clang-format`, so [its workflow](.github/workflows/clang-format.yml) is dispatch-only; enabling it means adopting a style and reformatting the tree.

The [Scorecard workflow](.github/workflows/scorecard.yml) cannot be required either: it runs on pushes to `main` and on a schedule, never on a pull request.

## Required status checks

The names to tick under branch protection, exactly as GitHub reports them:

| Check | Covers |
| :--- | :--- |
| `actionlint / actionlint` | workflow syntax |
| `apple_clang / all` | Xcode 16.4 and 26.6, Debug and Release |
| `clang / all` | Clang 22, 23, 24-SVN with libstdc++ |
| `clang_cl / all` | clang-cl on VS 2022, 2026, 2026-Preview |
| `clang_libcxx / all` | Clang 22, 23, 24-SVN with libc++ |
| `clang_tidy / all` | clang-tidy on all three rungs |
| `codeql / Analyze` | `security-extended` |
| `consumption / Consume` | the three CMake consumption models |
| `gcc / all` | GCC 15, 16, 17-SVN |
| `mingw / all` | MinGW 15 and 16 |
| `msvc / all` | cl on VS 2022, 2026, 2026-Preview |
| `msvc_analyze / all` | `/analyze` on all three rungs |
| `sanitizers / all` | all fifteen sanitizer legs |

## License

By contributing, you agree that your contributions will be licensed under the [Boost Software License, Version 1.0](LICENSE_1_0.txt), the same license that covers the rest of this repository.
