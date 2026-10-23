# Building and testing Arithvex

Arithvex is a C++20 mathematics engine with a shared library and an interactive console runner. The root CMake configuration builds on Windows and Linux. The legacy Visual Studio solution remains available for Windows developers.

## Prerequisites

- CMake 3.20 or newer
- C++20 toolchain (MSVC, GCC, or Clang)
- No third-party mathematics library is required for the current engine

## Windows (Visual Studio generator)

```powershell
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
.\build\Release\MathEngineRunner.exe
```

## Linux

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/MathEngineRunner
```

To build without tests, add `-DBUILD_TESTING=OFF` to the CMake configuration.

## Targets

| Target | Purpose |
| --- | --- |
| `MathEngineCore` | Static C++ implementation used by the shared library and tests |
| `MathEngine` | Shared library exposing the C-style ABI entry points |
| `MathEngineRunner` | Interactive command-line client |
| `arithvex_tests` | Regression tests, built when `BUILD_TESTING=ON` |

The public C ABI is declared in `The extra har level maths PHD Level++/MathEngineExports.h`. ABI functions that cannot produce a valid numerical result generally return NaN (or a zero result count), and text-producing functions signal failure with -1. Check individual API contracts before consuming results.

## Development quality gates

1. Run the Debug and Release CMake builds.
2. Execute CTest on Windows and Linux.
3. Test mathematical edge cases (non-real and repeated roots, finite probability inputs, unsigned bounds).
4. Preserve ABI structures and exported names when modifying the public header.
5. Do not commit local build artifacts such as .obj, .pch, .dll, or .exe.

GitHub Actions runs Debug and Release builds on both Ubuntu and Windows for pull requests targeting main.

## Known cleanup tasks

- The repository contains historical binaries and compiler intermediate files tracked by Git. .gitignore prevents newly untracked instances but does not untrack historical files; remove those in a dedicated, reviewed cleanup change.
- The original oversized README is preserved in `docs/archive/README-original-2026-10-23.md`; the root README now serves as the maintained project overview.
- Verify all existing mathematical algorithms with expanded numerical reference tests before declaring a stable release.
