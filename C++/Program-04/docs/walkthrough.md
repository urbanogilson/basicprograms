# How this project is put together

Notes on every file in the repo: what it does, and why it is written that way.
Read it next to the code, top to bottom, or jump to whichever file you are
looking at.

## Layout

```
include/tictactoe/   public headers
src/                 the library
app/                 the executable
tests/               GoogleTest suite
benchmark/           Google Benchmark suite
cmake/               reusable CMake modules
scripts/             the one-shot rename script
.github/workflows/   CI
.vscode/             editor run/debug config
```

The split that matters is **library, plus a thin executable that links it**.
`app/main.cpp` is four lines because everything real lives in the library. That
is what makes the code testable: tests link the library the same way the app
does. If the logic lived in `main.cpp`, no test could reach it.

## Part 1: the C++ code

### `include/tictactoe/example.h`

```cpp
#pragma once

namespace tictactoe {

int add(int a, int b);

}
```

Three decisions in seven lines.

**`#pragma once` instead of include guards.** The classic form is
`#ifndef TICTACTOE_EXAMPLE_H` / `#define ...` / `#endif`. `#pragma once` is not
in the C++ standard, but every compiler this project supports implements it, and
it cannot break the way a copy-pasted guard macro can when two headers end up
with the same name.

**The directory is part of the include path.** The file is at
`include/tictactoe/example.h`, not `include/example.h`, so users write
`#include "tictactoe/example.h"`. The `include/` directory is what gets added to
the compiler's search path, so the `tictactoe/` component is always spelled out
at the call site. That is what stops your `example.h` from colliding with
somebody else's, and it is why installed libraries look like
`#include <fmt/format.h>` rather than `#include <format.h>`.

**Declaration only, no body.** The header promises `add` exists; `src/example.cpp`
provides it. Callers recompile only when the promise changes, not when the
implementation does.

### `src/example.cpp`

```cpp
#include "tictactoe/example.h"

namespace tictactoe {

int add(int a, int b) {
    return a + b;
}

} // namespace tictactoe
```

The header comes first, on its own line, before any other include. That is a
deliberate habit: if `example.h` forgot an include it needs, this translation
unit is the one that catches it. Put `<iostream>` above it and the header could
be silently leaning on whatever `<iostream>` dragged in.

The `// namespace tictactoe` comment on the closing brace is a convention
clang-tidy's `readability` checks like, and it is genuinely useful once a
namespace runs past one screen.

### `app/main.cpp`

```cpp
#include "tictactoe/example.h"

#include <iostream>

int main() {
    std::cout << tictactoe::add(2, 3) << '\n';

    return 0;
}
```

Own header first, blank line, then standard library. clang-format's
`IncludeBlocks: Regroup` enforces that grouping.

`'\n'` rather than `std::endl`. `std::endl` writes a newline *and* flushes the
stream; in a loop that is a real cost, and `std::cout` flushes on exit anyway.
Reach for `std::endl` when you actually need the flush.

`return 0;` is optional in `main` (falling off the end means the same thing).
It is here because being explicit costs nothing.

### `tests/example_test.cpp`

```cpp
TEST(AddTest, AddsTwoNumbers) {
    EXPECT_EQ(tictactoe::add(2, 3), 5);
}
```

`TEST(SuiteName, TestName)` is a GoogleTest macro that defines a function and
registers it with the framework. No list of tests to maintain anywhere.

`EXPECT_EQ` versus `ASSERT_EQ`: `EXPECT_` records a failure and keeps going,
`ASSERT_` records it and returns from the test immediately. Use `ASSERT_` when
continuing would crash (a null pointer you are about to dereference), `EXPECT_`
otherwise, so one run reports every broken assertion instead of only the first.

The second test covers a negative input. Two cases is not thorough, but the
shape is the point: one obvious case, one case that could plausibly break.

### `benchmark/example_benchmark.cpp`

```cpp
namespace {

void bm_add(benchmark::State& state) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(tictactoe::add(2, 3));
    }
}

BENCHMARK(bm_add);

}
```

`for (auto _ : state)` is Google Benchmark's timing loop. The library decides how
many iterations to run and times the body; you do not pick an iteration count.

`DoNotOptimize` is the part worth remembering. `add(2, 3)` on two constants is
something any optimizer folds into `5` and then deletes, because nothing uses the
result. You would be timing an empty loop and reporting a suspiciously fast
number. `DoNotOptimize` tells the compiler the value escapes, so the call has to
actually happen.

The anonymous `namespace { }` gives everything inside internal linkage, so these
symbols cannot collide with anything in another translation unit. It is the
modern replacement for `static` at file scope.

## Part 2: the build

### `CMakeLists.txt` (root)

The top-level file, in the order it runs.

```cmake
cmake_minimum_required(VERSION 3.28)
```

This is not a formality. It sets the *policy* level, which controls how CMake
behaves in dozens of places where the semantics changed over the years.

The number is not arbitrary either. It is the highest version any feature in the
project actually requires, and it is worth knowing which feature sets it:

| Feature | Needs |
| --- | --- |
| `FILE_SET HEADERS` in `target_sources` | 3.23 |
| `FIND_PACKAGE_ARGS` in `FetchContent_Declare` | 3.24 |
| `SYSTEM` in `FetchContent_Declare` | 3.25 |
| `workflowPresets` (presets schema v6) | 3.25 |
| `EXCLUDE_FROM_ALL` in `FetchContent_Declare` | **3.28** |

Declaring a lower minimum than the highest entry in that table is a real bug, not
a nit: the file would use a keyword the declared version does not understand, and
older CMake would quietly do something other than what it says.

```cmake
project(tictactoe
    VERSION 0.1.0
    DESCRIPTION "A small modern C++ project template"
    LANGUAGES CXX
)
```

`project()` sets `PROJECT_NAME`, `PROJECT_VERSION`, `tictactoe_VERSION`, and
`PROJECT_IS_TOP_LEVEL`, and triggers compiler detection. `LANGUAGES CXX` skips
probing for a C compiler this project never uses.

```cmake
if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(FATAL_ERROR "...")
endif()
```

Checked once, right after compiler detection. Everything downstream can then
write GCC/Clang flags with no compiler branch of its own. "GNU|Clang" also
matches `AppleClang`, which is what you get from Xcode's toolchain.

```cmake
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

All three matter:

- `CXX_STANDARD 20` asks for C++20.
- `STANDARD_REQUIRED ON` makes it an error rather than a suggestion. Without it,
  an older compiler quietly falls back to whatever it supports.
- `EXTENSIONS OFF` gets you `-std=c++20` instead of `-std=gnu++20`, so GNU
  extensions are off and portable code stays portable.

```cmake
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

Writes `compile_commands.json` into the build directory: a list of every source
file and the exact flags used to compile it. clangd (editor completion and
diagnostics) and command-line clang-tidy both read it. Turning it on here means
you never have to remember the flag.

```cmake
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerWarnings)
include(Sanitizers)
include(Coverage)
include(StaticAnalysis)
```

`CMAKE_MODULE_PATH` is where `include(Foo)` looks for `Foo.cmake`. Each module
declares its own `option()` and defines its own functions, so the option and the
code that acts on it live in the same file.

```cmake
tictactoe_apply_sanitizers()
tictactoe_apply_coverage()

add_subdirectory(src)
add_subdirectory(app)
```

Order is load-bearing. Both of those call `add_compile_options()`, which sets a
*directory* property, and a target only picks up the directory's options as they
stood when the target was created. Move these two lines below `add_subdirectory`
and the instrumentation silently applies to nothing.

```cmake
if(PROJECT_IS_TOP_LEVEL)
```

True when this project is the one being built, false when somebody has pulled it
into their own build with `add_subdirectory` or `FetchContent`. Tests, the
coverage target, and the install rules all live inside this guard, so a consumer
who embeds the library does not inherit your test suite or your install rules.

```cmake
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif()
```

`include(CTest)` (rather than the shorter `enable_testing()`) defines
`BUILD_TESTING`, an option that defaults to ON. That gives anyone a documented
switch: `-DBUILD_TESTING=OFF` skips building tests, and therefore skips
downloading GoogleTest.

The install block at the bottom is what makes the library consumable by other
projects:

- `install(TARGETS tictactoe EXPORT tictactoeTargets ...)` copies the built
  artifacts and records the target in an export set.
- `install(EXPORT ... NAMESPACE tictactoe::)` writes `tictactoeTargets.cmake`,
  which recreates the target, with its include directories and compile features
  attached, in the consumer's build.
- `configure_package_config_file` turns `cmake/tictactoeConfig.cmake.in` into the
  file `find_package(tictactoe)` actually looks for.
- `write_basic_package_version_file(... COMPATIBILITY SameMajorVersion)` means a
  consumer asking for 0.1 accepts 0.9 but not 1.0, following semver.

### `src/CMakeLists.txt`

```cmake
add_library(tictactoe example.cpp)
add_library(tictactoe::tictactoe ALIAS tictactoe)
```

The alias exists so in-tree code and out-of-tree code spell the target the same
way: `tictactoe::tictactoe`. There is a practical payoff. CMake treats any name
containing `::` as *definitely* a target, so a typo fails at configure time with
"target not found". Without the namespace, a typo becomes a link flag named
`myprojet` and you find out at link time, or worse, not at all.

```cmake
target_sources(tictactoe
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/include
        FILES ${PROJECT_SOURCE_DIR}/include/tictactoe/example.h
)
```

`FILE_SET HEADERS` (CMake 3.23+) replaces two older declarations at once. It
adds `include/` to the include path for anything linking this target, *and* it
tells `install()` where the headers go and what directory structure to preserve.
The older way needed a `target_include_directories` with a
`BUILD_INTERFACE`/`INSTALL_INTERFACE` generator-expression pair plus a separate
`install(DIRECTORY)`, and the two could drift apart.

```cmake
target_compile_features(tictactoe PUBLIC cxx_std_20)
```

`CMAKE_CXX_STANDARD` at the top set the standard for compiling *this* project.
This line records C++20 as a requirement *of the target*, so it propagates: a
consumer who links `tictactoe::tictactoe` is compiled as C++20 too. `PUBLIC`
means "applies to me and to whoever links me".

That `PRIVATE` / `PUBLIC` / `INTERFACE` distinction is the core idea of modern
CMake. `PRIVATE` is used to build this target only. `INTERFACE` is imposed on
consumers only. `PUBLIC` is both.

### `app/CMakeLists.txt`

```cmake
add_executable(tictactoe_app main.cpp)
target_link_libraries(tictactoe_app PRIVATE tictactoe::tictactoe)
```

`PRIVATE` because nothing links an executable, so there is nothing to propagate
to. The include path for `tictactoe/example.h` arrives through the link, carried
by the library's `PUBLIC` file set. `main.cpp` never mentions an include
directory.

### `tests/CMakeLists.txt`

```cmake
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    URL_HASH SHA256=40d4ec94...
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    SYSTEM
    EXCLUDE_FROM_ALL
    FIND_PACKAGE_ARGS NAMES GTest
)
```

Line by line:

- **`URL` with a pinned tag, not `GIT_REPOSITORY`.** An archive download is
  faster than a clone and does not need git present.
- **`URL_HASH`** is the security-relevant one. Without it you build whatever that
  URL serves today. With it, a changed archive fails the build.
- **`SYSTEM`** marks GoogleTest's headers as system headers, which suppresses
  warnings from them. This project's warning set is strict enough that
  third-party headers would otherwise bury your own warnings.
- **`EXCLUDE_FROM_ALL`** (CMake 3.28+) keeps GoogleTest's own targets out of the
  default build. It is worth measuring rather than assuming: drop it and a plain
  build also produces `libgmock.a` and `libgmock_main.a`, neither of which
  anything here links.
- **`FIND_PACKAGE_ARGS NAMES GTest`** (CMake 3.24+) makes this try
  `find_package(GTest)` first and only download if the system has none.

```cmake
gtest_discover_tests(tictactoe_tests)
```

The alternative, `add_test(NAME ... COMMAND tictactoe_tests)`, registers the
whole binary as one CTest test. `gtest_discover_tests` runs the binary at build
time with `--gtest_list_tests` and registers each case separately, which is why
`ctest` shows `AddTest.AddsTwoNumbers` and `AddTest.HandlesNegativeNumbers` as
two entries. You can then run one test, and a failure names itself.

That build-time discovery has a consequence worth knowing: until the test binary
is built, CTest holds a placeholder test called `tictactoe_tests_NOT_BUILT`. If
you ever see that name, something asked CTest to run before the tests compiled.

### `benchmark/CMakeLists.txt`

Same FetchContent shape as `tests/`, plus four cache variables forced off before
`FetchContent_MakeAvailable`:

```cmake
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "" FORCE)
```

This is the standard way to configure a dependency you are pulling into your own
build: set its options in your cache before its `CMakeLists.txt` runs, and its
`option()` calls will see a value already there. `FORCE` is needed because
`option()` will not overwrite an existing cache entry, but a stale one from a
previous configure would otherwise win.

The last one is worth understanding. Google Benchmark compiles itself with
`-Werror` by default, and a compiler newer than the release will emit warnings
its authors never saw, breaking *your* build over *their* code. Turning off a
dependency's `-Werror` is routine.

The target links `benchmark::benchmark_main`, which supplies `main()`. Link
plain `benchmark::benchmark` instead and you write your own.

### `cmake/CompilerWarnings.cmake`

```cmake
function(enable_compiler_warnings target)
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic ...)
```

A function taking a target, called explicitly on each first-party target, rather
than a project-wide `add_compile_options`. Directory-wide flags would hit
GoogleTest and Google Benchmark too.

The flags beyond `-Wall -Wextra` are the interesting ones:

| Flag | Catches |
| --- | --- |
| `-Wconversion`, `-Wsign-conversion` | implicit narrowing, and signed/unsigned mixing |
| `-Wshadow` | an inner variable hiding an outer one |
| `-Wnull-dereference` | a dereference the compiler can prove is null |
| `-Wdouble-promotion` | `float` silently widened to `double` |
| `-Wimplicit-fallthrough` | a `switch` case falling through without `[[fallthrough]]` |
| `-Wold-style-cast` | `(int)x` instead of `static_cast<int>(x)` |
| `-Wcast-align` | a cast that increases required alignment |
| `-Wnon-virtual-dtor` | a base class with virtuals and a non-virtual destructor |
| `-Woverloaded-virtual` | a derived function hiding a virtual instead of overriding it |

`-Wconversion` and `-Wsign-conversion` are the two that will actually make you
change how you write code, mostly around container sizes: `.size()` returns an
unsigned type, and comparing it to an `int` now warns.

`-Werror` is separate, behind `TICTACTOE_WARNINGS_AS_ERRORS`, on in the `dev`
preset and in CI. Warnings that are not errors are warnings you will scroll past.

### `cmake/Sanitizers.cmake` and `cmake/Coverage.cmake`

Both are directory-scope (`add_compile_options` + `add_link_options`) rather than
per-target, and the reason is the same: instrumentation has to cover *everything*
that links into one binary. An ASan-instrumented library linked against an
uninstrumented GoogleTest produces false container-overflow and ODR reports. So
this is a whole-build switch or nothing.

That is the asymmetry to take away. **Warnings are per-target because you only
want them on your own code. Sanitizers and coverage are per-directory because
partial application is worse than none.**

The sanitizer flags:

- `-fsanitize=address` catches use-after-free, buffer overflow, and leaks.
- `-fsanitize=undefined` catches signed overflow, bad shifts, misaligned or null
  pointer use.
- `-fno-omit-frame-pointer` keeps stack traces readable.

Both flags appear in *compile* and *link* options, which is a common mistake:
sanitizers need a runtime library linked in, so compile-only leaves you with
undefined symbols.

For coverage, `--coverage` is shorthand for `-fprofile-arcs -ftest-coverage`, and
`-O0` is there because optimized code cannot be mapped back to source lines
honestly.

`tictactoe_add_coverage_target()` then builds a `coverage` target that runs
`ctest` and renders a gcovr report. Two details in it:

```cmake
    if(TARGET tictactoe_tests)
        add_dependencies(coverage tictactoe_tests)
    endif()
```

`cmake --build . --target coverage` builds *only* that target. Without this
dependency, `ctest` runs before the test binary exists and you get the
`tictactoe_tests_NOT_BUILT` placeholder. The `if(TARGET ...)` guard keeps it
working when `BUILD_TESTING=OFF`.

The fallback branch, for when gcovr is not installed, uses `VERBATIM`. Without
it, CMake hands the command to the shell unescaped and the colon and dot in the
message produce a syntax error instead of the instruction you were trying to
print.

Note the two are also different in kind: `tictactoe_apply_coverage` is a `macro`
while `enable_compiler_warnings` is a `function`. A macro runs in the caller's
variable scope; a function gets its own. Here it makes no practical difference,
since `add_compile_options` sets a directory property either way. It would matter
the moment one of them tried to `set()` a variable for the caller to read.

### `cmake/StaticAnalysis.cmake`

```cmake
set_target_properties(${target} PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_EXECUTABLE};--extra-arg=-Wno-unknown-warning-option"
)
```

Setting the `CXX_CLANG_TIDY` *target property* rather than the global
`CMAKE_CXX_CLANG_TIDY` variable, so analysis covers first-party targets only.
CMake then runs clang-tidy as part of compiling each file, so findings appear in
build output like compiler diagnostics.

The `;` is CMake's list separator: the property is a command plus its arguments,
not a string with a space in it.

### `cmake/tictactoeConfig.cmake.in`

```cmake
@PACKAGE_INIT@
include("${CMAKE_CURRENT_LIST_DIR}/tictactoeTargets.cmake")
check_required_components(tictactoe)
```

The template that becomes `tictactoeConfig.cmake` at install time, which is the
file `find_package(tictactoe)` searches for. `@PACKAGE_INIT@` is expanded by
`configure_package_config_file` into a preamble that makes the package
relocatable, so it works wherever it was installed rather than only at the prefix
it was built for.

### `CMakePresets.json`

Presets replace long remembered command lines. Four sections:

- **`configurePresets`** carry the cache variables. A `base` preset marked
  `"hidden": true` holds what everything shares (generator, `binaryDir`), and the
  rest `inherit` from it. `binaryDir` is `build/${presetName}`, so every preset
  gets its own directory and switching between them never triggers a rebuild.
- **`buildPresets`** point at a configure preset.
- **`testPresets`** carry ctest settings such as `outputOnFailure`.
- **`workflowPresets`** chain configure, build, and test into one command. This
  is the section that requires schema v6 (CMake 3.25).

`dev`, `asan`, and `release` have workflows because you run them end to end.
`tidy`, `coverage`, and `bench` do not, because their payoff is a build artifact
or a report rather than a test run.

## Part 3: tooling

### `.clang-format`

`BasedOnStyle: LLVM` with a handful of overrides. `ColumnLimit: 100` and
`IndentWidth: 4` are taste. Two that are not:

- `PointerAlignment: Left` gives `int* p`, binding the `*` to the type.
- `IncludeBlocks: Regroup` with `SortIncludes: CaseSensitive` is what enforces
  the include ordering described above, automatically.

The value of any formatter config is that it exists and is checked in CI. Which
style loses its importance the moment nobody has to argue about it.

### `.clang-tidy`

Where clang-format handles layout, clang-tidy finds bugs and suggests better
constructs. The check list turns on broad families (`bugprone-*`, `cert-*`,
`modernize-*`, `performance-*`, `readability-*`) and then subtracts four that
fight readable code, including `readability-identifier-length` (which rejects
`i`, `x`, `a`, `b`) and `readability-magic-numbers` (which fires on every literal
in a test).

```yaml
WarningsAsErrors: 'bugprone-*,performance-*'
```

Everything is reported, but only the two families that indicate real defects
break the build. Promoting `*` would mean a stylistic nit fails CI, and the
predictable result is that somebody turns the whole thing off.

`HeaderFilterRegex` is required to get findings from headers at all: by default
clang-tidy reports only on the `.cpp` file it was handed. Note the key name.
`HeaderFilterExpression` looks plausible and is silently ignored.

### `.editorconfig`

Editor-level settings (charset, line endings, indent) that nearly every editor
reads without a plugin. It covers file types clang-format does not: YAML, JSON,
Markdown, and CMake. `root = true` stops the search walking further up the
filesystem.

### `.gitignore` and `.gitattributes`

`.gitignore` covers `build*/`, clangd's `.cache/`, and `CMakeUserPresets.json`.
That last one is the documented place for your own local presets, so it is
personal by design and should never be committed.

`.gitattributes` is one line, `* text=auto eol=lf`, which normalizes line endings
in the repository regardless of the platform someone clones onto.

### `.vscode/launch.json` and `.vscode/tasks.json`

Two debug configurations (the app, and the test binary) using `cppdbg` and gdb,
each with a `preLaunchTask` so F5 never runs a stale binary. `tasks.json` defines
that task as `cmake --preset dev && cmake --build build/dev`; it configures every
time because doing so is nearly free once the directory exists, and it means the
task works on a fresh clone.

### `.github/workflows/ci.yml`

Five jobs, each answering a different question:

| Job | Question |
| --- | --- |
| `build-test` | Does it build and pass on GCC and Clang, Debug and Release? |
| `sanitizers` | Does it do anything undefined at runtime? |
| `benchmarks` | Does the benchmark suite still compile and run? |
| `coverage` | What fraction of the code do the tests reach? |
| `lint` | Is it formatted, and does clang-tidy object? |

`build-test` uses a matrix of two compilers times two presets, so one push gives
four builds. `fail-fast: false` lets them all finish, so you see whether a
failure is one configuration or all of them.

The sanitizer job sets `UBSAN_OPTIONS: halt_on_error=1`, because UBSan's default
is to print a diagnostic and continue, which in CI means a green check on a
broken build.

The benchmark job runs with `--benchmark_min_time=0.01s`. It is checking that the
suite works, not measuring anything: timings on a shared runner are too noisy to
be meaningful.

### `scripts/rename.sh`

The one file that deletes itself. It replaces the two placeholder tokens
(`tictactoe` and `TICTACTOE`) everywhere, moves the paths carrying the name, and
strips the template section from the README.

As a shell script it is worth reading for a few habits:

- `set -eu` exits on any failing command and on any undefined variable.
- Input is validated against `^[a-z][a-z0-9_]*$` before anything is touched,
  because the name has to be legal as both a C++ namespace and a CMake target.
- It refuses to run if `include/tictactoe/` is already gone, so a second run
  cannot corrupt an already-renamed tree.
- `git ls-files --cached --others --exclude-standard` lists tracked *and*
  uncommitted files. Plain `git ls-files` lists only tracked ones, which silently
  skips anything you have added but not committed.
- `cat "$tmp" > "$file"` rather than `mv`, because `mv` would replace the file
  and lose its permission bits, and this script rewrites an executable.

## Ideas that recur

**Targets carry their own requirements.** Nothing in this project sets a global
include path or a global flag for its own code. The library knows it needs C++20
and where its headers are, and anything linking it inherits that. This is the
difference between modern CMake and the older variable-driven style.

**`PRIVATE` is for building me, `INTERFACE` is for my consumers, `PUBLIC` is
both.** Nearly every `target_*` command takes these, and picking the wrong one is
the most common CMake bug.

**Everything optional defaults to `OFF`.** A plain `cmake -S . -B build` gets a
plain build, with no sanitizers, no coverage, no clang-tidy, and no downloads for
tools you did not ask for. Presets turn things on.

**Dependencies are pinned by hash and preferred from the system.** Every
`FetchContent_Declare` has a `URL_HASH` and a `FIND_PACKAGE_ARGS`, so builds are
reproducible and a system copy wins over a download.

## Rough edges

Being honest about what is not right yet.

**The C++20 baseline is wider than the CMake one.** GCC 13 and Clang 18 are
available on plenty of systems whose CMake is older than 3.28, and those users
have to install a newer CMake (`pipx install cmake`) rather than use the distro
package. That is the price of `EXCLUDE_FROM_ALL`, paid knowingly.

**Coverage only measures what the tests run.** The report currently says 100%
because there is exactly one function and two tests. Treat that number as an
indicator, not a goal.

**`add(int, int)` overflows silently.** It is placeholder code, but worth noticing
as you replace it: signed overflow is undefined behaviour, and the UBSan preset
would catch it if a test ever passed it two large values.
