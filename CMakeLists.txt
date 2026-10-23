cmake_minimum_required(VERSION 3.20)

project(Arithvex VERSION 1.0.0 LANGUAGES CXX)

# The v1.0.0 stable artifacts are published only after the gated workflow passes.
set(ARITHVEX_PRERELEASE "")
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CTest)

# Opt-in Linux/GCC sanitizer instrumentation for a separate CI job.
option(ARITHVEX_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer (Linux/GCC)" OFF)
if(ARITHVEX_ENABLE_SANITIZERS)
    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux" OR NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(FATAL_ERROR "ARITHVEX_ENABLE_SANITIZERS currently requires Linux and GCC")
    endif()
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

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
target_include_directories(ArithvexCore PUBLIC
    "$<BUILD_INTERFACE:${ARITHVEX_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

add_library(Arithvex SHARED "${ARITHVEX_SOURCE_DIR}/MathEngineExports.cpp")
target_link_libraries(Arithvex PRIVATE ArithvexCore)
target_include_directories(Arithvex PUBLIC
    "$<BUILD_INTERFACE:${ARITHVEX_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
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

# CLI and shared-library runtime locations are platform-aware.
# On Windows, DLL and CLI both install to bin. On Linux, set a relocatable RPATH.
if(UNIX AND NOT APPLE)
    set_target_properties(ArithvexCLI PROPERTIES INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
endif()

install(TARGETS ArithvexCore Arithvex ArithvexCLI
    EXPORT ArithvexTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY "${ARITHVEX_SOURCE_DIR}/"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/licenses/Arithvex")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ArithvexConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ArithvexConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Arithvex"
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ArithvexConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(EXPORT ArithvexTargets
    FILE ArithvexTargets.cmake NAMESPACE Arithvex::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Arithvex"
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ArithvexConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ArithvexConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Arithvex"
)

# Stable archive filenames contain the numeric version, OS and architecture.
set(CPACK_PACKAGE_NAME "Arithvex")
set(CPACK_PACKAGE_VENDOR "Arithvex Contributors")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_FILE_NAME
    "Arithvex-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_GENERATOR "ZIP;TGZ")
include(CPack)
