# Architecture

Arithvex separates mathematical implementation, exported ABI and interactive presentation. This allows the numerical C++ code to be tested without depending on console I/O, while installed SDK consumers can link to a named shared library.

## Build graph

```text
src/Arithvex/{AlgebraStats,AdvancedComputation,Geometry3D,
             MathCore,SymbolicCalculus}.cpp
                    |
              ArithvexCore     (static C++ implementation)
                 /    \
                /      \
        arithvex_tests  Arithvex (shared library)
                             /           \
                       ArithvexCLI     external SDK consumer
```

`CMakeLists.txt` also installs the `ArithvexCore` static library, public `*.h` headers, CMake package exports, license and CLI. The installed package exposes `Arithvex::Arithvex` and `Arithvex::ArithvexCore`.

## Modules

| Location | Responsibility |
| --- | --- |
| [`MathCore.h`](../src/Arithvex/MathCore.h) | Vector primitives, constants, numerical formatting and checked arithmetic helpers |
| [`AlgebraStats.h`](../src/Arithvex/AlgebraStats.h) | Polynomial roots, integer combinatorics, probability and statistics |
| [`AdvancedComputation.h`](../src/Arithvex/AdvancedComputation.h) | Matrices, FFT, ODE solvers, eigenvalue algorithms and series |
| [`Geometry3D.h`](../src/Arithvex/Geometry3D.h) | Two- and three-dimensional geometric objects |
| [`DiscreteMath.h`](../src/Arithvex/DiscreteMath.h) | Set and mapping templates |
| [`SymbolicCalculus.h`](../src/Arithvex/SymbolicCalculus.h) | Expression wrappers and numerical calculus backend |
| [`MathEngineExports.h`](../src/Arithvex/MathEngineExports.h) | C-linkage exports used by the shared library and CLI |
| [`Arithvex.h`](../src/Arithvex/Arithvex.h) | Umbrella header and `namespace Arithvex = MathEngine` alias |
| [CLI source](../apps/ArithvexCLI/ArithvexCLI.cpp) | Interactive menu and input handling |
| [Regression tests](../tests/math_engine_tests.cpp) | Numerical correctness and error-case tests |

## Target identities and compatibility

| Target | Artifact or purpose |
| --- | --- |
| `ArithvexCore` | Static library for C++ implementation |
| `Arithvex` | Shared library exporting the existing `MathEngine*` ABI functions |
| `ArithvexCLI` | Interactive console executable |
| `arithvex_tests` | CTest regression executable when tests are enabled |
| `MathEngineCore`, `MathEngine`, `MathEngineRunner` | Source-tree CMake aliases, not separate buildable targets |

The library file is named `Arithvex.dll` on Windows and `libArithvex.so` on Linux. Historical `MathEngine` namespaces/symbol identifiers are retained for source/API compatibility; **binary filename compatibility is not promised**.

The `extern "C"` functions can be called by compatible C++ consumers without C++ name mangling, but ABI structures and `wchar_t` sizes make a universal cross-compiler C ABI an inaccurate claim. See [API guide](API.md) and [ABI notes](releases/v1.0.0-abi.md).

## Testing and packaging

`BUILD_TESTING=ON` enables CTest. Release-mode CI verifies an installed SDK consumer and distribution archives on Windows and Ubuntu. Linux GCC supports an opt-in AddressSanitizer/UndefinedBehaviorSanitizer configuration with `-DARITHVEX_ENABLE_SANITIZERS=ON`.

CMake's CPack configuration creates platform-specific ZIP/TGZ files. A prerelease archive can be built while stable v1.0.0 is still unpublished. Consult [releases](https://github.com/sanskarIN/Arithvex/releases) for actual published assets.

## Design boundaries

- Numerical calculus is implemented, not general symbolic simplification.
- Most internal methods calculate in `long double`, while the exported functions use `double`; do not promise bit-for-bit equality across operating systems.
- The C++ implementation classes have no separately guaranteed cross-compiler binary ABI.
- Interactive CLI prompts and outputs are presentation behavior, not a stable programmatic protocol.
