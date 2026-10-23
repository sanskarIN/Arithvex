#include <cmath>
#include <iostream>
#include "MathEngineExports.h"

int main()
{
    const auto solution = SolveQuadraticExport(1.0, -3.0, 2.0);
    if (solution.count != 2U)
    {
        std::cerr << "Arithvex consumer: unexpected root count\n";
        return 1;
    }

    const auto first = solution.roots[0].real;
    const auto second = solution.roots[1].real;
    const auto hasOne = std::abs(first - 1.0) < 1e-12 || std::abs(second - 1.0) < 1e-12;
    const auto hasTwo = std::abs(first - 2.0) < 1e-12 || std::abs(second - 2.0) < 1e-12;
    if (!hasOne || !hasTwo)
    {
        std::cerr << "Arithvex consumer: incorrect roots\n";
        return 1;
    }
    std::cout << "Arithvex installed SDK consumer passed.\n";
    return 0;
}
