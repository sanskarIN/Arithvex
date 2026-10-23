# Arithvex

**Engineering the Future of Mathematics.**

[![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus&logoColor=white)](https://en.cppreference.com/w/cpp/20)
[![Build and tests](https://github.com/sanskarIN/Arithvex/actions/workflows/c-cpp.yml/badge.svg?branch=main)](https://github.com/sanskarIN/Arithvex/actions/workflows/c-cpp.yml)
[![MIT License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg)](https://github.com/sanskarIN/Arithvex/issues)

Arithvex is an **open-source C++20 mathematical computation engine** featuring algebra, statistics, probability, numerical calculus, geometry, discrete mathematics, and advanced numerical methods. It provides a reusable mathematical core, a shared library with an `extern "C"` API, and an interactive command-line runner.

> **Project status:** Under active development. The Windows and Ubuntu Debug/Release builds and the initial regression suite pass in GitHub Actions. Numerical accuracy, portability beyond those platforms, and API stability still require broader verification. **A stable release is not claimed.**

## Contents

- [What it can do](#what-it-can-do)
- [Get started](#get-started)
- [Use the library](#use-the-library)
- [Project structure](#project-structure)
- [Testing and development status](#testing-and-development-status)
- [Contributing](#contributing)
- [License and links](#license-and-links)

## What it can do

| Area | Current functionality |
| --- | --- |
| **Algebra** | Quadratic and cubic root solving; quadratic factorization |
| **Statistics** | Mean, median, mode |
| **Probability** | Conditional probability, Bayes' theorem, joint and total probability |
| **Combinatorics** | Permutations and combinations, including checked 64-bit overflow reporting |
| **Numerical calculus** | Limits, first/second/partial derivatives, definite integration, anchored indefinite integrals |
| **Linear algebra** | Matrix utilities, power iteration, Jacobi eigendecomposition for symmetric matrices |
| **Signal processing** | Fast Fourier transform (FFT) and inverse FFT |
| **Numerical methods** | Euler and fourth-order Runge–Kutta ODE solvers; Taylor and Maclaurin approximations |
| **2D and 3D geometry** | Lines, circles, conics, planes, spheres, ellipsoids, paraboloids, hyperboloids |
| **Discrete mathematics** | Finite sets and set operations; mapping analysis (injective, surjective, bijective) |
| **Mathematical constants** | Numerical approximations of π and the golden ratio |

**Important distinction:** The `SymbolicCalculus` module currently uses a **numerical calculus backend** for its computations. The name should not be interpreted as a complete computer algebra system or general-purpose symbolic simplifier. Most internal calculations use `long double`; the public shared-library interface mainly exchanges `double`. Precision is platform- and algorithm-dependent.

## Get started

### Requirements

- CMake **3.20 or later**
- A **C++20** compiler (MSVC, GCC, or Clang)
- Windows or Linux for the currently verified builds

The default CMake build does not require an external mathematics library.

Clone the project:

```bash
git clone https://github.com/sanskarIN/Arithvex.git
cd Arithvex
```

### Build on Windows

In PowerShell with a supported Visual Studio C++ toolchain:

```powershell
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
.\build\Release\MathEngineRunner.exe
```

### Build on Linux

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/MathEngineRunner
```

The **interactive runner** lets you work with equations, statistics, probability, geometry, calculus, series approximations, eigenvalues, π, and the golden ratio through its numbered menu.

For troubleshooting, Debug builds, and detailed build instructions, see **[Building and testing Arithvex](docs/BUILDING.md)**. A legacy Visual Studio solution is also present for Windows developers.

## Use the library

The supported shared-library entry points are declared in [`MathEngineExports.h`](The%20extra%20har%20level%20maths%20PHD%20Level%2B%2B/MathEngineExports.h).

Example of calling the exported quadratic solver from C++:

```cpp
#include "MathEngineExports.h"
#include <iostream>

int main() {
    // x^2 - 3x + 2 = 0, with roots 1 and 2.
    const MathEngineQuadraticRoots result = SolveQuadraticExport(1.0, -3.0, 2.0);

    if (result.count != 2) {
        std::cerr << "Unable to solve the equation\n";
        return 1;
    }

    for (std::size_t i = 0; i < result.count; ++i) {
        std::cout << "Root " << i + 1 << ": "
                  << result.roots[i].real << " + "
                  << result.roots[i].imaginary << "i\n";
    }
}
```

Link against the `MathEngine` shared-library target, and make sure the runtime library is available when launching an application. When calling other exported methods, handle their documented invalid-input/error outcomes (such as `NaN`, a zero root count, or a negative status).

### Build targets

| CMake target | Role |
| --- | --- |
| `MathEngineCore` | Static library containing the C++ mathematical implementation |
| `MathEngine` | Shared library exposing the `extern "C"` entry points |
| `MathEngineRunner` | Interactive console application |
| `arithvex_tests` | C++ regression-test executable when `BUILD_TESTING=ON` |

## Project structure

```text
Arithvex/
├── .github/workflows/c-cpp.yml      # Windows / Ubuntu CI matrix
├── CMakeLists.txt                  # Cross-platform build configuration
├── MathEngineRunner/               # Legacy Visual Studio runner project
├── The extra har level maths PHD Level++/
│   ├── AdvancedComputation.{h,cpp}
│   ├── AlgebraStats.{h,cpp}
│   ├── DiscreteMath.h
│   ├── Geometry3D.{h,cpp}
│   ├── MathCore.{h,cpp}
│   ├── MathEngineExports.{h,cpp}
│   ├── MathEngineRunner.cpp
│   └── SymbolicCalculus.{h,cpp}
├── tests/math_engine_tests.cpp     # Regression tests
├── docs/
│   ├── BUILDING.md
│   └── archive/README-original-2026-10-23.md
└── LICENSE
```

The historical directory names remain unchanged to avoid disruptive path changes to existing Visual Studio project files.

## Testing and development status

The GitHub Actions workflow runs **Debug and Release** builds on both **Ubuntu and Windows** and executes the CTest suite on pull requests and pushes to `main`.

The initial regression tests cover:
- Real, complex, and repeated polynomial roots, including cubic residual checks
- Combinatorics and unsigned 64-bit edge cases
- Probability input validation (including non-finite values)
- Public ABI evaluation, invalid parameter counts, and basic statistics

Passing these tests **does not guarantee** every mathematical function is validated. Areas requiring continued work include deeper FFT/inverse FFT, matrix, eigenvalue, geometry, calculus, boundary-condition, and numerical-precision testing.

See the [open issues](https://github.com/sanskarIN/Arithvex/issues) for current tasks. Priority improvements include [expanding numerical tests](https://github.com/sanskarIN/Arithvex/issues/4), [removing previously tracked build artifacts](https://github.com/sanskarIN/Arithvex/issues/2), and [protecting the `main` branch](https://github.com/sanskarIN/Arithvex/issues/5).

## Contributing

Bug reports, reproducible mathematical counterexamples, documentation improvements, and C++ contributions are welcome.

1. Open an [issue](https://github.com/sanskarIN/Arithvex/issues) to describe a bug or proposed improvement.
2. Create a branch and keep your changes focused.
3. Include regression tests for numerical or public API changes.
4. Run CMake and CTest locally, then open a pull request against `main`.
5. Check all relevant GitHub Actions jobs before merging.

Avoid committing generated executable and compiler files; they belong in local build directories or appropriate release assets.

## License and links

Released under the [MIT License](LICENSE). Copyright information and license terms are maintained in the LICENSE file.

- **Repository:** https://github.com/sanskarIN/Arithvex
- **Project maintainer:** [Sanskar](https://github.com/sanskarIN)
- **Website:** https://sanskarin.github.io
- **Contact:** https://sanskarin.github.io/contact/#send-a-message
- **Support development:** [Buy Me a Coffee](https://buymeacoffee.com/SanskarCodes) · [Razorpay](https://razorpay.me/@SanskarCodes) · [PayPal](https://paypal.me/SanskarCodes)

**Previous README:** The original long-form README has been preserved verbatim at [docs/archive/README-original-2026-10-23.md](docs/archive/README-original-2026-10-23.md) for historical reference. It contains repetitive legacy material and is **not** authoritative documentation of current capabilities.
