# Contributing to Arithvex

Contributions to Arithvex are welcome, particularly reproducible numerical bugs, regression tests, accessibility improvements in the CLI and corrections to the developer documentation.

## Before changing code

1. Read the [Quick start](QUICKSTART.md) and make sure a local CMake/CTest run succeeds.
2. Search the [open issues](https://github.com/sanskarIN/Arithvex/issues) and [pull requests](https://github.com/sanskarIN/Arithvex/pulls) for similar work.
3. For a larger change or a potential ABI incompatibility, describe your proposal in an issue before implementing it.

## Work in a branch

Branch from an up-to-date `main`, make a small, reviewable change, and open a pull request back into `main`. The repository protects `main`; avoid force-pushing there. Do not submit build outputs (`.obj`, `.dll`, `.exe`, `.pdb`, `CMakeCache.txt` etc.).

The maintained source layout is explained in [Architecture](ARCHITECTURE.md). Keep public symbol names and installed CMake targets stable unless your proposal includes migration steps and compatibility review.

## Local validation

**Linux, Release:**

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

**Windows, Visual Studio/PowerShell:**

```powershell
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
```

**Linux GCC memory/undefined-behavior instrumentation (opt-in):**

```bash
cmake -S . -B build-sanitized -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DARITHVEX_ENABLE_SANITIZERS=ON
cmake --build build-sanitized --parallel
ctest --test-dir build-sanitized --output-on-failure
```

## Testing mathematical changes

Each algorithm change should include at least one reference-value test, an invalid-input or boundary test when relevant, and a clear tolerance for floating-point comparisons. Existing regression examples are in [`tests/math_engine_tests.cpp`](../tests/math_engine_tests.cpp). See [Numerical behavior](NUMERICAL_BEHAVIOR.md) for precision caveats.

For ABI changes, use [`tests/consumer/`](../tests/consumer/) to validate an **installed** library separately from source-tree linking. Check both Windows and Ubuntu CI because `wchar_t`, `long double` and DLL/SO linking behavior are platform dependent.

## Improving documentation

- Edit the relevant page and update [the documentation index](README.md) if you add or rename a guide.
- Keep CMake examples copyable, with accurate platform-specific paths and target names.
- Link to current source headers and maintained guides rather than duplicating entire headers.
- Do not promise stable releases, binaries or supported operating systems until the corresponding GitHub release and CI evidence exist.
- Do not restore removed historical README archives or add separate contact-page URLs; the project's main website is https://sanskarin.github.io.

## Pull request checklist

- [ ] Change is scoped and the motivation is explained
- [ ] New public behavior has documentation and tests where applicable
- [ ] Windows and Linux Debug/Release CTest checks are green
- [ ] ABI and install/package compatibility are considered
- [ ] No generated binaries, local paths, secrets or credentials are committed
- [ ] New documentation links point to existing source or docs files

For additional questions, use the [GitHub issue tracker](https://github.com/sanskarIN/Arithvex/issues).
