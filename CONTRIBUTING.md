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
- **Line and branch coverage are both 100%, project-wide and for the PR's own diff.** The [Coverage workflow](.github/workflows/coverage.yml) passes `--fail-under-line 100 --fail-under-branch 100`, and [`codecov.yml`](.github/codecov.yml) sets both Codecov statuses to a 100% target with zero tolerance. New code needs a test that exercises every line and branch it adds; existing coverage may not regress.

  Excluded from that bar, by the shared workflow: `assert(...)` contract checks, compiler-synthesized `= default;` members, the exception-unwinding branches gcov attaches to any call that could throw, and a line holding nothing but a closing brace — which carries no code of yours, only the unwind resume for a function with a local to destroy.

  **Code that cannot be tested does not get an exemption; it gets commented out.** If you cannot reach a line from a test, comment it out and say why, so that whoever uncomments it has to write the test. One arm in the library is unreachable rather than untested, and is written on one line with the reason beside it: `bit_array<0>::is_valid`, only ever called from an `assert` that a zero-size array has no member to reach, and not removable because MSVC's `/W4` rejects a bare `n < N` as always false when `N` is zero.

  Keep an `assert` on a line of its own, as the library does. The workflow drops assert branches by matching the start of a line, so an assert sharing a line with real code keeps a branch no test can take.

One check runs without being required:

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
| `coverage / gcovr` | 100% of lines and branches |
| `gcc / all` | GCC 15, 16, 17-SVN |
| `mingw / all` | MinGW 15 and 16 |
| `msvc / all` | cl on VS 2022, 2026, 2026-Preview |
| `msvc_analyze / all` | `/analyze` on all three rungs |
| `sanitizers / all` | all fifteen sanitizer legs |

Codecov posts two more, `codecov/project` and `codecov/patch`, which carry the same 100% bar for the whole tree and for the diff.

## License

By contributing, you agree that your contributions will be licensed under the [Boost Software License, Version 1.0](LICENSE_1_0.txt), the same license that covers the rest of this repository.
