#include <algorithm>
#include <cmath>
#include <complex>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "Arithvex.h"
#include "AlgebraStats.h"
#include "MathEngineExports.h"

namespace
{
void require(bool condition, const std::string& message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

bool near(double lhs, double rhs, double tolerance = 1.0e-9)
{
    return std::abs(lhs - rhs) <= tolerance;
}

void testQuadratic()
{
    const auto result = SolveQuadraticExport(1.0, -3.0, 2.0);
    require(result.count == 2, "quadratic must return two roots");
    std::vector<double> roots { result.roots[0].real, result.roots[1].real };
    std::sort(roots.begin(), roots.end());
    require(near(roots[0], 1.0) && near(roots[1], 2.0), "incorrect real quadratic roots");

    const auto imaginary = SolveQuadraticExport(1.0, 0.0, 1.0);
    require(imaginary.count == 2, "complex quadratic must return two roots");
    require(near(std::abs(imaginary.roots[0].imaginary), 1.0), "incorrect complex quadratic root");
    require(near(std::abs(imaginary.roots[1].imaginary), 1.0), "incorrect complex quadratic root");

    const auto repeated = SolveQuadraticExport(1.0, 0.0, 0.0);
    require(repeated.count == 2 && near(repeated.roots[0].real, 0.0)
        && near(repeated.roots[1].real, 0.0), "repeated quadratic root failed");
}

void checkCubic(double a, double b, double c, double d)
{
    const auto result = SolveCubicExport(a, b, c, d);
    require(result.count == 3, "cubic must return three roots");

    for (std::size_t i = 0; i < result.count; ++i)
    {
        const std::complex<double> root { result.roots[i].real, result.roots[i].imaginary };
        const auto residual = ((a * root + b) * root + c) * root + d;
        require(std::isfinite(root.real()) && std::isfinite(root.imag())
            && std::abs(residual) < 1.0e-8, "cubic root fails polynomial residual");
    }
}

void testCubic()
{
    checkCubic(1.0, 0.0, 1.0, 1.0);   // One real root and complex conjugates
    checkCubic(1.0, -6.0, 11.0, -6.0); // Three distinct real roots
    checkCubic(1.0, 0.0, 0.0, 0.0);   // Triple root at zero
    checkCubic(1.0, 0.0, -3.0, 2.0);  // Repeated non-zero roots
}

void testCombinatorics()
{
    using MathEngine::AlgebraStats::combinations;
    const auto maximum = (std::numeric_limits<std::uint64_t>::max)();
    const auto edge = combinations(maximum, 1);
    require(!edge.overflowed && edge.value == maximum, "nCr(max_uint64, 1) must not wrap");
    const auto small = combinations(10, 3);
    require(!small.overflowed && small.value == 120, "10 choose 3 must equal 120");
    const auto overflow = combinations(100, 50);
    require(overflow.overflowed, "100 choose 50 must report uint64 overflow");
}

void testProbabilities()
{
    using MathEngine::AlgebraStats::Probability;
    require(near(static_cast<double>(Probability::conditional(0.2L, 0.4L)), 0.5),
        "conditional probability mismatch");
    require(near(static_cast<double>(Probability::bayes(0.75L, 0.4L, 0.5L)), 0.6),
        "Bayes probability mismatch");

    bool invalidIntersection = false;
    try { (void)Probability::conditional(0.6L, 0.3L); }
    catch (const std::out_of_range&) { invalidIntersection = true; }
    require(invalidIntersection, "intersection cannot exceed conditioning event");

    bool invalidBayes = false;
    try { (void)Probability::bayes(1.0L, 1.0L, 0.1L); }
    catch (const std::out_of_range&) { invalidBayes = true; }
    require(invalidBayes, "inconsistent Bayes inputs must be rejected");

    bool invalidNaN = false;
    try { (void)Probability::joint(std::numeric_limits<long double>::quiet_NaN(), 0.5L); }
    catch (const std::out_of_range&) { invalidNaN = true; }
    require(invalidNaN, "NaN is not a valid probability");
}

void testAdvancedMath()
{
    using namespace Arithvex::AdvancedComputation;

    const std::vector<std::complex<long double>> impulse {
        { 1.0L, 0.0L }, { 0.0L, 0.0L }, { 0.0L, 0.0L }, { 0.0L, 0.0L }
    };
    const auto spectrum = fft(impulse);
    require(spectrum.size() == impulse.size(), "FFT must preserve power-of-two length");
    for (const auto value : spectrum)
    {
        require(std::abs(value - std::complex<long double> { 1.0L, 0.0L }) < 1.0e-12L,
            "FFT impulse response must be constant");
    }
    const auto restored = ifft(spectrum);
    for (std::size_t i = 0; i < restored.size(); ++i)
    {
        require(std::abs(restored[i] - impulse[i]) < 1.0e-12L,
            "inverse FFT must restore original values");
    }

    const auto padded = fft({ { 1.0L, 0.0L }, { 2.0L, 0.0L }, { 3.0L, 0.0L } });
    require(padded.size() == 4, "non-power-of-two FFT should zero-pad to 4");
    const auto paddedRestore = ifft(padded);
    require(std::abs(paddedRestore[0] - std::complex<long double> { 1.0L, 0.0L }) < 1.0e-12L
        && std::abs(paddedRestore[2] - std::complex<long double> { 3.0L, 0.0L }) < 1.0e-12L
        && std::abs(paddedRestore[3]) < 1.0e-12L, "zero-padded FFT round trip mismatch");

    auto identity = Matrix::identity(2);
    require(identity(0, 0) == 1.0L && identity(1, 1) == 1.0L && identity(0, 1) == 0.0L,
        "matrix identity must have unit diagonal");
    bool rejectedIndex = false;
    try { (void)identity(2, 0); }
    catch (const std::out_of_range&) { rejectedIndex = true; }
    require(rejectedIndex, "matrix must reject out-of-range indices");

    Matrix diagonal(2, 2);
    diagonal(0, 0) = 4.0L;
    diagonal(1, 1) = 2.0L;
    const auto dominant = powerIteration(diagonal);
    require(std::abs(dominant.eigenvalue - 4.0L) < 1.0e-6L,
        "dominant eigenvalue of diagonal matrix must be 4");

    const auto trajectory = ODESolver::rungeKutta4(
        [](long double, const ODESolver::State& state) { return state; },
        0.0L, { 1.0L }, 0.1L, 10U);
    require(trajectory.size() == 11U
        && std::abs(trajectory.back().state[0] - std::exp(1.0L)) < 3.0e-6L,
        "RK4 approximation for y'=y must approach e");
}

void testGeometryInvariants()
{
    using namespace Arithvex::Geometry3D;

    const Arithvex::Vector2 first { -1.0L, -1.0L };
    const Arithvex::Vector2 second { 2.0L, 2.0L };
    const auto line = StraightLine::fromPoints(first, second);
    const auto evaluateLine = [&](const Arithvex::Vector2& point)
    {
        return line.a() * point.x + line.b() * point.y + line.c();
    };
    require(std::abs(evaluateLine(first)) < 1.0e-12L
        && std::abs(evaluateLine(second)) < 1.0e-12L,
        "line from two points must contain both points");

    bool rejectedCoincidentPoints = false;
    try { (void)StraightLine::fromPoints(first, first); }
    catch (const std::invalid_argument&) { rejectedCoincidentPoints = true; }
    require(rejectedCoincidentPoints, "coincident points must not define a line");

    const Circle circle({ 1.0L, -2.0L }, 3.0L);
    require(circle.center().x == 1.0L && circle.center().y == -2.0L
        && circle.radius() == 3.0L, "circle must retain center and radius");

    const Sphere sphere({ 1.0L, 2.0L, 3.0L }, 4.0L);
    require(sphere.center().z == 3.0L && sphere.radius() == 4.0L,
        "sphere must retain center and radius");

    bool rejectedNegativeRadius = false;
    try { (void)Sphere({ 0.0L, 0.0L, 0.0L }, -1.0L); }
    catch (const std::invalid_argument&) { rejectedNegativeRadius = true; }
    require(rejectedNegativeRadius, "sphere must reject negative radius");

    const Ellipse ellipse({ 0.0L, 0.0L }, 5.0L, 3.0L);
    require(std::abs(ellipse.focalDistance() - 4.0L) < 1.0e-12L,
        "ellipse focal distance must satisfy c^2 = a^2 - b^2");
    const auto foci = ellipse.foci();
    require(std::abs(foci.first.x + 4.0L) < 1.0e-12L
        && std::abs(foci.second.x - 4.0L) < 1.0e-12L,
        "ellipse foci must lie along its major axis");

    const Plane plane({ 1.0L, 2.0L, 3.0L }, { 0.0L, 0.0L, 2.0L });
    require(std::abs(plane.normal().z - 1.0L) < 1.0e-12L
        && std::abs(plane.d() + 3.0L) < 1.0e-12L,
        "plane through a point must use normalized implicit coefficients");
}

void testNumericalCalculus()
{
    using namespace Arithvex::SymbolicCalculus;

    const Expression square("x^2", [](const VariableMap& variables)
    {
        const auto x = variables.at("x");
        return x * x;
    });
    const CalculusEngine engine;
    require(std::abs(engine.derivative(square, "x", 3.0L) - 6.0L) < 1.0e-5L,
        "derivative of x^2 at x=3 must be 6");
    require(std::abs(engine.secondDerivative(square, "x", 3.0L) - 2.0L) < 1.0e-4L,
        "second derivative of x^2 must be 2");

    require(std::abs(engine.integrateDefinite(square, "x", 0.0L, 3.0L) - 9.0L) < 1.0e-5L,
        "integral of x^2 from 0 to 3 must be 9");
    require(std::abs(engine.integrateDefinite(square, "x", 3.0L, 0.0L) + 9.0L) < 1.0e-5L,
        "reversed integration bounds must negate the integral");
    require(engine.integrateDefinite(square, "x", 2.0L, 2.0L) == 0.0L,
        "equal integration bounds must return zero");

    const auto antiderivative = engine.integrateIndefinite(square, "x", 0.0L);
    require(std::abs(antiderivative.evaluate({ { "x", 3.0L } }) - 9.0L) < 1.0e-5L,
        "anchored antiderivative must reproduce the definite integral");

    const Expression xy("x*y", [](const VariableMap& variables)
    {
        return variables.at("x") * variables.at("y");
    });
    require(std::abs(engine.partialDerivative(xy, "x", { { "x", 2.0L }, { "y", 3.0L } }) - 3.0L) < 1.0e-5L,
        "partial derivative of x*y with respect to x must equal y");

    bool missingVariable = false;
    try { (void)engine.partialDerivative(xy, "x", { { "y", 3.0L } }); }
    catch (const std::invalid_argument&) { missingVariable = true; }
    require(missingVariable, "partial derivative must reject absent target variable");
}

void testReleaseBoundaryGuards()
{
    const auto maximumSize = (std::numeric_limits<std::size_t>::max)();
    const auto maximumUnsigned = (std::numeric_limits<std::uint64_t>::max)();

    bool rejectedMatrixOverflow = false;
    try { (void)Arithvex::AdvancedComputation::Matrix(maximumSize, 2U); }
    catch (const std::overflow_error&) { rejectedMatrixOverflow = true; }
    require(rejectedMatrixOverflow, "matrix allocation must reject size_t dimension overflow");

    bool rejectedHugeOde = false;
    try {
        (void)Arithvex::AdvancedComputation::ODESolver::euler(
            [](long double, const std::vector<long double>& y) { return y; },
            0.0L, { 1.0L }, 1.0L, maximumSize);
    }
    catch (const std::overflow_error&) { rejectedHugeOde = true; }
    require(rejectedHugeOde, "ODE trajectory length must reject size_t overflow");

    const auto impossibleCombination = Arithvex::AlgebraStats::combinations(maximumUnsigned, maximumUnsigned / 2U);
    require(impossibleCombination.overflowed && impossibleCombination.value == 0U,
        "giant nCr must report overflow without allocating enormous vectors");
    const auto ordinaryCombination = Arithvex::AlgebraStats::combinations(8U, 4U);
    require(!ordinaryCombination.overflowed && ordinaryCombination.value == 70U,
        "nCr overflow guard must preserve ordinary results");

    using namespace Arithvex::Geometry3D;
    const auto nan = (std::numeric_limits<long double>::quiet_NaN)();
    const auto inf = (std::numeric_limits<long double>::infinity)();
    bool rejectedNaNRadius = false;
    try { (void)Circle({ 0.0L, 0.0L }, nan); }
    catch (const std::invalid_argument&) { rejectedNaNRadius = true; }
    require(rejectedNaNRadius, "circle must reject NaN radius");

    bool rejectedInfiniteRadius = false;
    try { (void)Sphere({ 0.0L, 0.0L, 0.0L }, inf); }
    catch (const std::invalid_argument&) { rejectedInfiniteRadius = true; }
    require(rejectedInfiniteRadius, "sphere must reject infinite radius");

    bool rejectedNonFiniteLine = false;
    try { (void)StraightLine(inf, 1.0L, 0.0L); }
    catch (const std::invalid_argument&) { rejectedNonFiniteLine = true; }
    require(rejectedNonFiniteLine, "line must reject non-finite coefficients");

    bool rejectedNonFiniteNormal = false;
    try { (void)Arithvex::normalize({ 1.0L, nan, 1.0L }); }
    catch (const std::invalid_argument&) { rejectedNonFiniteNormal = true; }
    require(rejectedNonFiniteNormal, "vector normalization must reject NaN");
}

void testSpectralAndConstants()
{
    using namespace Arithvex::AdvancedComputation;
    require(fft({}).empty(), "FFT of an empty input must stay empty");
    const auto singleton = ifft(fft({ { 2.0L, -3.0L } }));
    require(singleton.size() == 1U && std::abs(singleton[0] - std::complex<long double> { 2.0L, -3.0L }) < 1.0e-12L,
        "single-element FFT round trip must preserve complex values");

    Matrix symmetric(2, 2);
    symmetric(0, 0) = 2.0L;
    symmetric(0, 1) = 1.0L;
    symmetric(1, 0) = 1.0L;
    symmetric(1, 1) = 2.0L;
    const auto decomposition = jacobiEigenDecomposition(symmetric);
    require(decomposition.eigenvalues.size() == 2U
        && std::abs(decomposition.eigenvalues[0] - 3.0L) < 1.0e-9L
        && std::abs(decomposition.eigenvalues[1] - 1.0L) < 1.0e-9L,
        "Jacobi eigendecomposition must produce eigenvalues 3 and 1");
    for (std::size_t i = 0U; i < 2U; ++i)
    {
        const auto x = decomposition.eigenvectors(0, i);
        const auto y = decomposition.eigenvectors(1, i);
        const auto lambda = decomposition.eigenvalues[i];
        require(std::abs(2.0L * x + y - lambda * x) < 1.0e-9L
            && std::abs(x + 2.0L * y - lambda * y) < 1.0e-9L,
            "Jacobi eigenvectors must satisfy A*v = lambda*v");
    }

    const auto pi = std::acos(-1.0L);
    require(std::abs(computePi(5U) - pi) < 1.0e-12L,
        "Gauss-Legendre approximation must converge to pi");
    const auto golden = (1.0L + std::sqrt(5.0L)) / 2.0L;
    require(std::abs(computeGoldenRatio(50U) - golden) < 1.0e-12L,
        "golden ratio continued fraction must converge");
}

void testExports()
{
    MathEngineFunctionSpec polynomial {};
    polynomial.kind = MathFunctionPolynomial;
    polynomial.parameterCount = 3;
    polynomial.parameters[0] = 1.0;
    polynomial.parameters[1] = 2.0;
    polynomial.parameters[2] = 3.0;
    require(near(EvaluateFunctionExport(&polynomial, 2.0), 17.0),
        "polynomial ABI evaluation mismatch");

    polynomial.parameterCount = MATHENGINE_MAX_FUNCTION_PARAMETERS + 1;
    require(std::isnan(EvaluateFunctionExport(&polynomial, 2.0)),
        "oversized parameter count must be rejected");
    require(DescribeFunctionExport(&polynomial, nullptr, 0) == -1,
        "invalid function description must be rejected");

    const double values[] { 1.0, 2.0, 3.0, 4.0 };
    require(near(MeanExport(values, 4), 2.5), "mean ABI mismatch");
    require(near(MedianExport(values, 4), 2.5), "median ABI mismatch");
    require(std::isnan(MeanExport(nullptr, 4)), "null values must yield NaN");
}
} // namespace

int main()
{
    try
    {
        testQuadratic();
        testCubic();
        testCombinatorics();
        testProbabilities();
        testAdvancedMath();
        testGeometryInvariants();
        testNumericalCalculus();
        testReleaseBoundaryGuards();
        testSpectralAndConstants();
        testExports();
        std::cout << "Arithvex regression tests passed.\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "Arithvex regression test failed: " << error.what() << '\n';
        return 1;
    }
}
