# tictactoe

A C++20 CMake setup: strict warnings, sanitizers, benchmarks, coverage, clang-tidy, clang-format and CI. One library (`tictactoe`), one executable (`tictactoe_app`), one GoogleTest suite.

- `include/tictactoe/` public headers, installed via `FILE_SET HEADERS`
- `src/` the `tictactoe` library
- `app/` the `tictactoe_app` executable
- `tests/` GoogleTest suite, discovered by CTest
- `benchmark/` Google Benchmark suite (opt-in)
- `cmake/` warning, sanitizer, coverage and static-analysis modules

## Requirements

- CMake 3.28+ and a C++20 compiler: GCC 13+ or Clang 18+, on Linux or macOS.
  Windows and MSVC are not supported, and the build stops with an error on any
  other compiler.
- Optional: `gcovr` for coverage reports (`pipx install gcovr`)
- The presets use Unix Makefiles. Change `"generator"` in `CMakePresets.json` if you prefer Ninja.

## Everyday use

Each workflow configures, builds and tests in one command:

| Command                             | What it does                               |
| ----------------------------------- | ------------------------------------------ |
| `cmake --workflow --preset dev`     | Debug build, warnings as errors, run tests |
| `cmake --workflow --preset asan`    | Same, instrumented with ASan + UBSan       |
| `cmake --workflow --preset release` | RelWithDebInfo build, run tests            |

Builds land in `build/<preset>/`, so presets never clobber each other.

```console
$ ./build/dev/app/tictactoe_app
5
```

`tidy` and `coverage` are configure-and-build rather than workflows:

```bash
# Static analysis over first-party sources only
cmake --preset tidy && cmake --build build/tidy

# Coverage report -> build/coverage/coverage/index.html
cmake --preset coverage && cmake --build build/coverage --target coverage
```

Formatting is checked in CI and applied locally with:

```bash
clang-format -i src/*.cpp app/*.cpp include/tictactoe/*.h tests/*.cpp benchmark/*.cpp
```

## Benchmarks

Off by default, so `--preset dev` never downloads a dependency it will not run.

```bash
cmake --preset bench && cmake --build build/bench
./build/bench/benchmark/tictactoe_benchmarks
```

`bench` builds Release because Google Benchmark warns about debug builds and debug timings are useless. Useful flags: `--benchmark_filter=<regex>`, `--benchmark_min_time=1s`, `--benchmark_format=json`.

## Options

All default to `OFF`, so a plain `cmake -S . -B build` stays plain.

| Option                         | Effect                                           |
| ------------------------------ | ------------------------------------------------ |
| `TICTACTOE_WARNINGS_AS_ERRORS` | Adds `-Werror` to first-party targets            |
| `TICTACTOE_ENABLE_SANITIZERS`  | ASan + UBSan across the whole build              |
| `TICTACTOE_ENABLE_COVERAGE`    | gcov instrumentation, plus the `coverage` target |
| `TICTACTOE_ENABLE_CLANG_TIDY`  | Runs clang-tidy during compilation               |
| `TICTACTOE_ENABLE_BENCHMARKS`  | Builds the Google Benchmark suite                |

Two things in `cmake/` are not symmetric, and both are on purpose:

- Warnings are per-target. The set is strict enough to bury first-party findings under GoogleTest noise, so `enable_compiler_warnings()` is only called on our own targets.
- Sanitizers and coverage are directory-scope. Instrumentation has to cover everything linked into one binary. Mixing an instrumented library with an uninstrumented GoogleTest gives false container-overflow and ODR reports.

`compile_commands.json` is always generated, so clangd and clang-tidy work without extra flags.

## Dependencies

GoogleTest and Google Benchmark are pinned by SHA-256 and fetched with `FetchContent`. Both use `FIND_PACKAGE_ARGS`, so an installed copy wins and the download only happens when there is none. To force the download:

```bash
cmake --preset dev -DFETCHCONTENT_TRY_FIND_PACKAGE_MODE=NEVER
```
