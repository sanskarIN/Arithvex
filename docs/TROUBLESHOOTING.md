# Troubleshooting

Use this guide for development builds, installed SDK consumers and the CLI. When reporting a bug, include the exact command, full error text, operating system and compiler version.

## Common problems

| Symptom | What to check |
| --- | --- |
| `cmake: command not found` | Install CMake 3.20+ and reopen your terminal so the executable is on `PATH`. |
| CMake cannot identify a C++ compiler | Install a C++20 toolchain; on Windows use **Developer PowerShell/Command Prompt for Visual Studio** or explicitly choose a generator. |
| CMake cache points at another source directory or generator | Remove the locally generated `build/` directory, then configure from scratch with `cmake -S . -B build ...`. Do not commit local CMake caches. |
| C++ standard features such as `std::numbers` unavailable | Confirm C++20 support and an up-to-date compiler. CMake sets C++20 for targets, but older compilers may not implement it fully. |
| `ArithvexCLI.exe` not found on Windows | With a Visual Studio multi-configuration generator, check `build/Release/ArithvexCLI.exe`; with a single-configuration generator, check `build/ArithvexCLI.exe`. |
| `./build/ArithvexCLI` does not exist on Linux | Build with `cmake --build build --parallel`; inspect the output location if you configured a custom generator/output directory. |
| `find_package(Arithvex CONFIG REQUIRED)` fails | Install the engine first and pass the installation prefix via `-DCMAKE_PREFIX_PATH=/absolute/path/to/install` or set `Arithvex_DIR` to the directory holding `ArithvexConfig.cmake`. |
| `Arithvex::Arithvex` target missing | Make sure you used `find_package(Arithvex CONFIG REQUIRED)` in the consumer project; plain aliases like `MathEngine` are defined for source-tree builds only. |
| Missing `MathEngineExports.h` | Use the installed target's include directories via `target_link_libraries(app PRIVATE Arithvex::Arithvex)`. Avoid hardcoding old legacy source paths. |
| Windows executable cannot locate `Arithvex.dll` | Copy the DLL from `install/bin` alongside the consumer executable, or add that directory to `PATH`. |
| Linux loader cannot locate `libArithvex.so` | Use a loader-visible install location, application RPATH, or temporarily set `LD_LIBRARY_PATH=/path/to/install/lib`. |
| CMake refuses `ARITHVEX_ENABLE_SANITIZERS` | That opt-in sanitizer configuration currently supports **Linux with GCC**, not every compiler/OS. |
| CTest fails unexpectedly | Run `ctest --test-dir build --output-on-failure` (add `-C Release` for Visual Studio), and compare the failing calculation with the regression suite. |
| An exported numerical function returns NaN | Check null pointers, empty arrays, parameter counts, invalid mathematical domains, and numerical edge cases. Consult [API guide](API.md). |
| A root/FFT/eigenvalue differs by a small amount | Compare against a mathematically justified absolute/relative tolerance instead of expecting exact floating-point equality. See [Numerical behavior](NUMERICAL_BEHAVIOR.md). |

## Diagnostic commands

**Linux:**

```bash
cmake --version
c++ --version
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

**Windows, Developer PowerShell:**

```powershell
cmake --version
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --config Debug --parallel
ctest --test-dir build -C Debug --output-on-failure
```

If you already built with one generator, use a fresh build directory before switching generators.

## When to open an issue

A useful report includes:

- A minimal input or example, including expected and actual results
- OS, CPU architecture, compiler name/version, and CMake version
- Configure/build/test commands and the relevant terminal output
- Whether the failure is in `ArithvexCLI`, `ArithvexCore`, a shared-library export, or the installed SDK
- For numeric errors, a mathematical reference and appropriate tolerance

Open an issue at [Arithvex Issues](https://github.com/sanskarIN/Arithvex/issues), and consult [Contributing](CONTRIBUTING.md) if you want to prepare a patch.
