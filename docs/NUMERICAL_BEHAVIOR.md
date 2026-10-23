# Numerical behavior and precision

Arithvex is a **floating-point numerical engine**. Its implementations use finite-precision arithmetic and approximations, not exact symbolic manipulation. This guide documents behavior that can surprise users even when an operation succeeds.

## Precision models

- Most C++ implementation functions use `long double` internally.
- Exported shared-library entry points predominantly accept or return `double`.
- `long double` has different precision on different target compilers/architectures. Results can differ slightly across Windows and Linux, even with equivalent input.
- `Arithvex::kEpsilon` is a small internal threshold (`1e-12L`), **not** a promise that every computation is accurate within that amount.

## Comparing computed values

Avoid using `expected == actual` with floating-point outputs. Choose tolerances based on the algorithm and magnitude of the values.

```cpp
#include <algorithm>
#include <cmath>

bool near(long double a, long double b,
          long double absTol = 1e-10L,
          long double relTol = 1e-9L)
{
    return std::abs(a - b) <=
        std::max(absTol, relTol * std::max(std::abs(a), std::abs(b)));
}
```

This illustration assumes finite inputs; it does not treat NaN and infinities as equal. Record both the reference value and the tolerance in numerical regression tests.

## Algorithm-specific notes

| Area | Key behavior and limitations |
| --- | --- |
| Quadratic/cubic roots | Roots may be complex, repeated, or numerically close. A root should generally be checked by its polynomial **residual**, not exact equality to a rounded reference value. |
| Combinatorics | `permutations` and `combinations` report 64-bit integer overflow using `CheckedUnsigned64::overflowed`. Overflow must not silently wrap. |
| Probability | Inputs represent probabilities in [0, 1]. Invalid or inconsistent inputs can throw internally; exported operations normally return NaN. A total-probability partition mathematically requires priors summing to 1; callers should supply normalized priors. |
| FFT/IFFT | A non-power-of-two input is zero-padded to the next power of two. `ifft(fft(values))` returns that padded length, with rounding errors and near-zero imaginary components possible. Empty input returns an empty sequence. |
| Eigenvalues | `powerIteration` estimates a dominant eigenpair; convergence may depend on eigenvalue separation and the starting vector. `jacobiEigenDecomposition` is intended for symmetric matrices, not general unsymmetric input. |
| Euler / RK4 | ODE trajectories depend on step size and derivative smoothness. RK4 has higher order than Euler but is not universally stable; step sizes require convergence checks. |
| Derivatives | Finite-difference stencils can be inaccurate near discontinuities, singularities or severe cancellation. |
| Integrals | Adaptive Simpson integration estimates a definite integral numerically; discontinuities/singularities can invalidate error expectations. |
| Limits | A numerical estimate of approaching values is not a proof that a mathematical limit exists. |
| Series | Taylor/Maclaurin approximations depend on the number of terms and on the function's convergence properties. |
| Geometry | Degenerate parameters and non-finite dimensions can be rejected; transformed shape equations are formatted as approximate numeric text. |

## Special values and invalid inputs

- `NaN` is **not** a valid probability input.
- `NaN` and infinity may propagate from poorly conditioned mathematical operations; consumers should check `std::isfinite` before treating a result as valid.
- Matrix dimensions, indexes, and requested allocations can fail for zero, inconsistent, or excessive sizes.
- Negative or zero geometric dimensions are invalid where positive lengths are required. Some features intentionally allow signed focal parameters to encode direction.
- A function parameter count greater than 16 is invalid in the exported `MathEngineFunctionSpec`.

## Regression test policy

When making a numerical change:

1. Add a normal reference example (with known mathematical answer).
2. Test zero/degenerate inputs and invalid input behavior.
3. Include a finite-value check where overflow or domain issues are possible.
4. Test boundary cases (for example, very large `uint64_t`, non-power-of-two FFT lengths, non-symmetric matrices, reversed integration bounds).
5. Run CTest on Windows and Linux; when changing memory-heavy operations or numeric inputs, run the supported Linux GCC sanitizer configuration.

Current test code is in [`tests/math_engine_tests.cpp`](../tests/math_engine_tests.cpp). The presence of a test does not imply that all functions are exhaustively validated; consult [the release readiness checklist](releases/v1.0.0-readiness.md).
