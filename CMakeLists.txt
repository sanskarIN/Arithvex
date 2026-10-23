cmake_minimum_required(VERSION 3.20)

project(Arithvex VERSION 0.1.0 LANGUAGES CXX)
include(CTest)

set(ARITHVEX_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/Arithvex")

# The core stays separate so C++ tests can link it without exporting
# implementation internals from the C-ABI shared library.
add_library(ArithvexCore STATIC
    "${ARITHVEX_SOURCE_DIR}/AdvancedComputation.cpp"
    "${ARITHVEX_SOURCE_DIR}/AlgebraStats.cpp"
    "${ARITHVEX_SOURCE_DIR}/Geometry3D.cpp"
    "${ARITHVEX_SOURCE_DIR}/MathCore.cpp"
    "${ARITHVEX_SOURCE_DIR}/SymbolicCalculus.cpp"
)
set_target_properties(ArithvexCore PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_features(ArithvexCore PUBLIC cxx_std_20)
target_include_directories(ArithvexCore PUBLIC "${ARITHVEX_SOURCE_DIR}")

add_library(Arithvex SHARED "${ARITHVEX_SOURCE_DIR}/MathEngineExports.cpp")
target_link_libraries(Arithvex PRIVATE ArithvexCore)
target_include_directories(Arithvex PUBLIC "${ARITHVEX_SOURCE_DIR}")
target_compile_features(Arithvex PUBLIC cxx_std_20)
target_compile_definitions(Arithvex PRIVATE ARITHVEX_EXPORTS)

add_executable(ArithvexCLI "apps/ArithvexCLI/ArithvexCLI.cpp")
target_link_libraries(ArithvexCLI PRIVATE Arithvex)

# Compatibility aliases for downstream CMake users. These names are not
# buildable --target aliases; use Arithvex, ArithvexCore and ArithvexCLI.
add_library(MathEngineCore ALIAS ArithvexCore)
add_library(MathEngine ALIAS Arithvex)
add_executable(MathEngineRunner ALIAS ArithvexCLI)

if(BUILD_TESTING)
    add_executable(arithvex_tests tests/math_engine_tests.cpp)
    target_link_libraries(arithvex_tests PRIVATE ArithvexCore Arithvex)
    add_test(NAME arithmetic_regressions COMMAND arithvex_tests)
endif()

if(MSVC)
    target_compile_options(ArithvexCore PRIVATE /W4)
    target_compile_options(Arithvex PRIVATE /W4)
else()
    target_compile_options(ArithvexCore PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(Arithvex PRIVATE -Wall -Wextra -Wpedantic)
endif()
