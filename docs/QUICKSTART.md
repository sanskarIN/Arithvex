# Quick start

Use this guide to build Arithvex, verify a calculation through its interactive CLI, and use its exported library from a separate C++ project.

## Requirements

- CMake **3.20+**, a C++20 compiler, and Git
- Windows: Visual Studio Build Tools/MSVC and a **Developer PowerShell** or Developer Command Prompt
- Linux: GCC or Clang with C++20 and a working build tool such as Make or Ninja

The ordinary build does not require an external mathematics library.

## Clone and build

```bash
git clone https://github.com/sanskarIN/Arithvex.git
cd Arithvex
```

**Linux:**

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/ArithvexCLI
```

**Windows, Visual Studio generator (PowerShell):**

```powershell
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
.\build\Release\ArithvexCLI.exe
```

If you use Ninja or another single-configuration generator on Windows, the executable layout may differ; see [Building and testing](BUILDING.md) and [Troubleshooting](TROUBLESHOOTING.md).

## Try the interactive CLI

The menu includes quadratic/cubic equations, statistics, conditional probability, geometry, π, numerical calculus, series, matrices and the golden ratio.

For a first check, choose **1** (quadratic equation) and supply coefficients `a=1`, `b=-3`, `c=2`. The mathematical roots should be **1 and 2** (ordering may differ). Choose **0** to exit. Menu prompts may vary by operation.

## Use the installed SDK from another project

With the engine already built, install headers, libraries, the CLI and the CMake package configuration:

```bash
cmake --install build --config Release --prefix install
```

On Windows, the `--config Release` flag selects the Release build for multi-configuration generators. On Linux it is harmless.

Create a separate folder, for example `my-arithvex-app/`, with these two files:

**`CMakeLists.txt`**

```cmake
cmake_minimum_required(VERSION 3.20)
project(MyArithvexApp LANGUAGES CXX)

find_package(Arithvex 1.0 CONFIG REQUIRED)

add_executable(my_arithvex_app main.cpp)
target_compile_features(my_arithvex_app PRIVATE cxx_std_20)
target_link_libraries(my_arithvex_app PRIVATE Arithvex::Arithvex)
```

**`main.cpp`**

```cpp
#include <algorithm>
#include <cmath>
#include <iostream>
#include "MathEngineExports.h"

int main()
{
    const auto result = SolveQuadraticExport(1.0, -3.0, 2.0);
    if (result.count != 2) {
        std::cerr << "Unexpected number of roots\n";
        return 1;
    }

    const double a = result.roots[0].real;
    const double b = result.roots[1].real;
    if (std::abs(a - 1.0) > 1e-10 && std::abs(b - 1.0) > 1e-10) return 1;
    if (std::abs(a - 2.0) > 1e-10 && std::abs(b - 2.0) > 1e-10) return 1;
    std::cout << "Roots: " << a << ", " << b << '\n';
}
```

**Configure and build** (from your separate app folder):

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/absolute/path/to/Arithvex/install
cmake --build build --config Release
```

Replace the prefix with the **absolute** path to the engine's `install` folder. On Windows, use a path such as `C:/dev/Arithvex/install`. Run the resulting executable with the shared library available:

- On **Windows**, put `Arithvex.dll` (from `install/bin`) next to the consumer executable, or ensure its directory is in `PATH`.
- On **Linux**, add `install/lib` to the loader search path (for example with `LD_LIBRARY_PATH`) or configure a suitable application RPATH.

The repository includes an automatically built and executed consumer example at [`tests/consumer/`](../tests/consumer/). If that example or your app fails, use the [troubleshooting guide](TROUBLESHOOTING.md).

## Next steps

- [API guide](API.md) — C++ and shared-library entry points
- [Numerical behavior](NUMERICAL_BEHAVIOR.md) — precision, rounding and invalid input
- [Contributing](CONTRIBUTING.md) — running tests and submitting fixes

The SDK is a **C++ consumer** integration. Despite the `extern "C"` linkage of exports, the header is not a portable plain-C header.
