cmake_minimum_required(VERSION 3.20)

project(Arithvex VERSION 0.1.0 LANGUAGES CXX)

include(CTest)

set(ARITHVEX_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/The extra har level maths PHD Level++")

# Keep the mathematical implementation reusable without exposing its C++ internals
# from the stable C-ABI DLL. Tests link to this static core directly.
add_library(MathEngineCore STATIC
    "${ARITHVEX_SOURCE_DIR}/AdvancedComputation.cpp"
    "${ARITHVEX_SOURCE_DIR}/AlgebraStats.cpp"
    "${ARITHVEX_SOURCE_DIR}/Geometry3D.cpp"
    "${ARITHVEX_SOURCE_DIR}/MathCore.cpp"
    "${ARITHVEX_SOURCE_DIR}/SymbolicCalculus.cpp"
)
set_target_properties(MathEngineCore PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_features(MathEngineCore PUBLIC cxx_std_20)
target_include_directories(MathEngineCore PUBLIC "${ARITHVEX_SOURCE_DIR}")

add_library(MathEngine SHARED "${ARITHVEX_SOURCE_DIR}/MathEngineExports.cpp")
target_link_libraries(MathEngine PRIVATE MathEngineCore)
target_include_directories(MathEngine PUBLIC "${ARITHVEX_SOURCE_DIR}")
target_compile_features(MathEngine PUBLIC cxx_std_20)
target_compile_definitions(MathEngine PRIVATE THEEXTRAHARLEVELMATHSPHDLEVEL_EXPORTS)

add_executable(MathEngineRunner "${ARITHVEX_SOURCE_DIR}/MathEngineRunner.cpp")
target_link_libraries(MathEngineRunner PRIVATE MathEngine)

if(BUILD_TESTING)
    add_executable(arithvex_tests tests/math_engine_tests.cpp)
    target_link_libraries(arithvex_tests PRIVATE MathEngineCore MathEngine)
    add_test(NAME arithmetic_regressions COMMAND arithvex_tests)
endif()

if(MSVC)
    target_compile_options(MathEngineCore PRIVATE /W4)
    target_compile_options(MathEngine PRIVATE /W4)
else()
    target_compile_options(MathEngineCore PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(MathEngine PRIVATE -Wall -Wextra -Wpedantic)
endif()
