# API guide

This is a **developer-oriented map**, not a substitute for the signatures in [`MathEngineExports.h`](../src/Arithvex/MathEngineExports.h) and the underlying C++ headers.

## Choose an interface

| If you need... | Use | Link |
| --- | --- | --- |
| Source-level C++ mathematical objects and algorithms | `#include "Arithvex.h"` | `ArithvexCore` in the source tree, or `Arithvex::ArithvexCore` from the installed SDK |
| A shared-library API for quadratic roots, statistics, selected numerical functions and geometry descriptions | `#include "MathEngineExports.h"` | `Arithvex` in the source tree, or `Arithvex::Arithvex` from the installed SDK |
| Menu-driven calculations | `ArithvexCLI` | Run the executable |

**Important:** The public exported functions use `extern "C"` linkage, but the header uses C++ declarations such as `std::size_t`; it is not guaranteed usable directly from a C compiler. Symbol naming is deliberately historical: `MathEngine*` names remain while target/library names are `Arithvex`. For binary compatibility caveats, see [the platform/ABI contract](releases/v1.0.0-abi.md).

## C++ mathematical modules

Include `Arithvex.h` to expose the alias `namespace Arithvex = MathEngine`. The existing `MathEngine::` names continue to work.

| Namespace | Major operations |
| --- | --- |
| `Arithvex::AlgebraStats` | `solveQuadratic`, `solveCubic`, `factorQuadratic`, `permutations`, `combinations`, `mean`, `median`, `mode`, `Probability` |
| `Arithvex::AdvancedComputation` | `Matrix`, `fft`, `ifft`, `ODESolver`, `powerIteration`, `jacobiEigenDecomposition`, `computePi`, `computeGoldenRatio` |
| `Arithvex::Geometry3D` | `StraightLine`, `Circle`, `Parabola`, `Ellipse`, `Hyperbola`, `Plane`, `Sphere`, `Ellipsoid`, `Paraboloid`, `Hyperboloid` |
| `Arithvex::SymbolicCalculus` | `Expression`, `VariableMap`, `CalculusEngine`, `NumericCalculusBackend` |
| `Arithvex::DiscreteMath` | `Set<T>`, `analyzeMapping` |
| `Arithvex` | `Vector2`, `Vector3`, `dot`, `norm`, `normalize`, numeric formatting helpers |

### Example: checked combinatorics

```cpp
#include "Arithvex.h"
#include <iostream>

int main()
{
    const auto result = Arithvex::AlgebraStats::combinations(10, 3);
    if (result.overflowed) {
        std::cerr << "Result exceeds uint64_t\n";
        return 1;
    }
    std::cout << result.value << '\n'; // 120
}
```

The `CheckedUnsigned64` result contains both `value` and `overflowed`. Do not interpret `value == 0` as an overflow indicator without also checking the flag.

### Example: matrix and FFT

```cpp
#include "Arithvex.h"
#include <complex>
#include <vector>

int main()
{
    using namespace Arithvex::AdvancedComputation;
    Matrix matrix = Matrix::identity(2);
    matrix(0, 1) = 3.0L;

    const std::vector<std::complex<long double>> signal = {
        {1.0L, 0.0L}, {0.0L, 0.0L},
        {0.0L, 0.0L}, {0.0L, 0.0L}
    };
    const auto frequencies = fft(signal);
    const auto restored = ifft(frequencies);
    return frequencies.size() == restored.size() && matrix(0, 1) == 3.0L ? 0 : 1;
}
```

FFT pads non-power-of-two lengths with zeros, so its inverse returns the **padded** vector; it does not automatically restore the original unpadded length.

## Exported shared-library functions

The exact names and parameter types are in [the ABI header](../src/Arithvex/MathEngineExports.h).

| Group | Exported functions | Result / error convention |
| --- | --- | --- |
| Roots | `SolveQuadraticExport`, `SolveCubicExport` | `MathEngineQuadraticRoots` or `MathEngineCubicRoots`; `count == 0` if invalid |
| Statistics | `MeanExport`, `MedianExport` | `double`; NaN for invalid inputs |
| Probability | `BayesExport`, `ConditionalProbabilityExport` | `double`; NaN for invalid inputs |
| Constants | `ComputePiExport`, `ComputeGoldenRatioExport` | Approximation as `double` |
| Geometry | `GetCircleEquationExport`, `GetPlaneEquationExport` | Required `wchar_t` buffer length including terminator; `-1` for invalid geometry |
| General functions | `EvaluateFunctionExport`, `DifferentiateFunctionExport`, `SecondDerivativeFunctionExport`, `IntegrateFunctionExport`, `LimitFunctionExport` | `double`; generally NaN on failure |
| Descriptions | `DescribeFunctionExport` | Required wide-buffer length including terminator; `-1` on invalid spec |
| Series | `MaclaurinExpExport`, `MaclaurinSinExport`, `MaclaurinCosExport` | Numeric series approximation |
| Matrices | `DominantEigenvalueExport` | Dominant eigenvalue estimate or invalid result |

The `MathEngineFunctionSpec` structure holds `kind`, `parameters[16]` and `parameterCount`. Supported families are polynomial, sine, cosine, exponential, logarithm, reciprocal, power, and Gaussian (see `MathEngineFunctionKind`). Polynomial coefficients are in ascending power order: `parameters[0] + parameters[1]*x + ...`.

For example, `f(x) = 3 + 2x + x²` uses `MathFunctionPolynomial`, `parameterCount=3`, and `parameters={3,2,1}`. Always reject input counts larger than `MATHENGINE_MAX_FUNCTION_PARAMETERS`.

### Example: dynamic wide-text output

```cpp
#include "MathEngineExports.h"
#include <iostream>
#include <vector>

int main()
{
    const int required = GetCircleEquationExport(0.0, 0.0, 5.0, nullptr, 0);
    if (required <= 0) return 1;

    std::vector<wchar_t> buffer(static_cast<std::size_t>(required));
    const int actual = GetCircleEquationExport(
        0.0, 0.0, 5.0, buffer.data(), buffer.size());
    if (actual <= 0) return 1;

    std::wcout << buffer.data() << L'\n';
}
```

Wide strings use `wchar_t`, whose width differs on Windows and Linux. If a caller supplies a shorter buffer, the function truncates its text, null-terminates the supplied buffer, and returns the **required** length, not necessarily the amount written. Do not assume an arbitrary non-null pointer is safe to use.

## Numerical error handling

- **Internal C++ algorithms** often throw `std::invalid_argument`, `std::out_of_range`, `std::overflow_error`, etc.
- **Exported functions** catch implementation exceptions and generally return NaN, a zero root count, or `-1` for invalid text/equation requests. Check each signature.
- Compute tolerances explicitly, with `std::abs(actual - expected) <= tolerance` or relative tolerances when appropriate, rather than exact floating-point equality.

See [Numerical behavior](NUMERICAL_BEHAVIOR.md) and the runnable [external consumer](../tests/consumer/main.cpp) for additional examples.
