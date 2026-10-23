# Arithvex documentation

Arithvex is a C++20 mathematical engine with three ways to use it: an interactive command-line application, reusable C++ classes, and an exported shared-library interface.

## Choose a guide

| Goal | Start here |
| --- | --- |
| Build, launch the CLI and run your first calculation | [Quick start](QUICKSTART.md) |
| Configure CMake and troubleshoot compiler/platform differences | [Build and test](BUILDING.md) |
| Integrate the installed SDK into another C++ project | [Quick start: installed SDK](QUICKSTART.md#use-the-installed-sdk-from-another-project) |
| Find C++ modules, exported functions and error conventions | [API guide](API.md) |
| Understand precision, tolerances and numerical limitations | [Numerical behavior](NUMERICAL_BEHAVIOR.md) |
| Understand the source and CMake target layout | [Architecture](ARCHITECTURE.md) |
| Fix common build, install, runtime or CMake errors | [Troubleshooting](TROUBLESHOOTING.md) |
| Submit a patch, regression test or documentation improvement | [Contributing](CONTRIBUTING.md) |
| Review v1.0.0 readiness and platform limitations | [v1.0.0 checklist](releases/v1.0.0-readiness.md) and [ABI/platform notes](releases/v1.0.0-abi.md) |

## API entry points

- **C++ class API:** Include [`Arithvex.h`](../src/Arithvex/Arithvex.h), which provides the `Arithvex` namespace alias for the original `MathEngine` implementation.
- **Shared-library exports:** Include [`MathEngineExports.h`](../src/Arithvex/MathEngineExports.h) and link the `Arithvex` shared library. The exported names retain their historical `MathEngine` naming.
- **Command line:** Build or install `ArithvexCLI`; choose an operation from its numbered interactive menu.

Arithvex is **not** a general symbolic algebra system. Its `SymbolicCalculus` module currently implements numerical differentiation, integration and limit estimation. ABI compatibility across different compilers/architectures is not guaranteed.

## Source of truth

Current implementation lives in [`src/Arithvex/`](../src/Arithvex/). The build graph is defined by [`CMakeLists.txt`](../CMakeLists.txt) and CI configuration by [`.github/workflows/c-cpp.yml`](../.github/workflows/c-cpp.yml).

The stable-release workflow is being prepared separately. Before assuming a download is published, check the repository's [GitHub Releases](https://github.com/sanskarIN/Arithvex/releases) page. The documentation does not make an unpublished release available.

Found an outdated example? Open a [documentation issue](https://github.com/sanskarIN/Arithvex/issues), preferably with the command, operating system and compiler that reproduce it.
