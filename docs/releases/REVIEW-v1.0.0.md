# Arithvex v1.0.0 — final reviewer checklist

**Purpose:** Help a maintainer decide whether to approve the first stable release. This file is a review record, not evidence that a GitHub release has already been published.

## What you are reviewing

- [Release PR #12](https://github.com/sanskarIN/Arithvex/pull/12) — stable versioning, release notes, final validation and conditional publishing
- [Release notes](v1.0.0.md) — user-facing description and limitations
- [Platform/ABI contract](v1.0.0-abi.md) — exactly what existing integrations can expect
- [Stable-release workflow](../../.github/workflows/publish-v1.0.0.yml) — package/test/publish gates
- [Source CI](../../.github/workflows/c-cpp.yml) — Windows/Linux matrix and sanitizer regression tests

The release is for **Windows x64 / MSVC** and **Linux x86_64 / GCC**. Those packages contain the CLI, C++ SDK and shared library; they are portable ZIP archives, not installers.

## Evidence already collected

The release-preview run [39902089108](https://github.com/sanskarIN/Arithvex/actions/runs/39902089108) completed successfully at commit `85bdb9e8dab5b62ec79795ebc48f0d89acc9021a`. It executed both OS packaging jobs and Linux sanitizer validation. Its publish job was **skipped**, as required for pull requests. Two per-platform ZIP workflow artifacts and checksum files were uploaded.

The C++ CI run [39902089320](https://github.com/sanskarIN/Arithvex/actions/runs/39902089320) also passed all five checks for the same release branch snapshot.

**Important:** These are historical checkpoints. Review **the latest PR head and its own checks**, since the release branch and documentation can change before approval. Do not infer new revisions are verified merely because an earlier workflow succeeded.

## Checks for the final PR head

- [ ] The current pull request shows all required Windows/Linux Debug and Release checks **green**
- [ ] Linux GCC AddressSanitizer/UndefinedBehaviorSanitizer check is green
- [ ] Both platform packaging/installation jobs in the release-preview workflow are green
- [ ] Publication is **skipped** during pull-request validation
- [ ] The documentation link check passes after the documentation PR has merged
- [ ] No separate contact-page URL, old 2 MB README archive or broken file references remain
- [ ] Stable CMake project/package version is `1.0.0`, **not** `1.0.0-rc.1`
- [ ] Both download filenames contain `1.0.0` and the appropriate platform/CPU designation
- [ ] Each ZIP contains `ArithvexCLI`, `Arithvex` shared library, `MathEngineExports.h`, `ArithvexConfig.cmake`, and the MIT license
- [ ] Extracted ZIP CLI and installed external SDK consumer actually execute
- [ ] Platform SHA-256 manifests check out; the publisher combines them and rechecks both ZIPs
- [ ] API changes, numerical precision disclaimers and unsupported platforms are acceptable
- [ ] No critical unresolved correctness or security regression blocks v1.0.0

## Key compatibility decisions

- C++ symbols in namespace `MathEngine` and the `Arithvex` namespace alias remain; exported names are unchanged
- Shared library binary filename changed from `MathEngine` to `Arithvex`, so older consumers may need to relink
- `MathEngineExports.h` is intended for **C++** consumers despite the functions having `extern "C"` linkage
- No 32-bit, ARM, macOS or cross-compiler ABI compatibility is promised
- The `SymbolicCalculus` module performs **numerical** calculus, not full symbolic simplification
- Some invalid shared-library inputs use NaN / zero root count / `-1`; callers must check results

## Approval and publication sequence

1. Review [PR #12](https://github.com/sanskarIN/Arithvex/pull/12), including this checklist, release notes and the latest successful checks.
2. Approve and merge the PR into protected `main` only when all required checks are satisfied.
3. The publisher is designed to run **on that reviewed push to `main`**, and only after it independently rebuilds, tests, packages, extracts and verifies both native archives and passes sanitizers.
4. If any publication gate fails, **do not claim v1.0.0 is released**; repair and re-run the verification.
5. Confirm a public [v1.0.0 GitHub release](https://github.com/sanskarIN/Arithvex/releases) exists with both ZIP assets and `SHA256SUMS.txt`.
6. Verify the published tag points to the intended reviewed source commit and the associated `main` tests are green.
7. Only after that, close the [stable release tracker #9](https://github.com/sanskarIN/Arithvex/issues/9) and announce verified release download links.

**Until step 5 has been checked, the release should be described as *review-ready*, not *published*.**
