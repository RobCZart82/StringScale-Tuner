# Repository review — 2026-11-05

Reviewed baseline: `82058e8c8ae878d12ba5a83bce77bfbdd87a2cf7`.

## Findings and changes

1. **Package path validation (medium, defensive hardening).** The shared validator
   used POSIX path rules but its results also feed Windows filesystem operations.
   It accepted `C:/outside`, drive-relative `C:outside`, NTFS stream syntax and
   noncanonical aliases. The validator now rejects colons, backslashes, parent
   traversal, absolute paths, `.` and noncanonical spellings. A regression test
   exercises rejected and valid paths. Normal generated inventories are unchanged.
   This review does not demonstrate a remote exploit: package inputs are produced
   by the project's own build workflow.
2. **Mixed-language documentation.** The default README, roadmap and branding
   README mixed English and Hungarian. Default pages are now English, with
   language-specific Hungarian counterparts. English build/download/installer/
   release guides and a documentation index provide a complete starting path.
   Hungarian historical evidence, draft release notes and user-facing bilingual
   package text are preserved. The manuals index now links to the correct guide.
3. **Historical validation ambiguity.** TEST_RESULTS.md called the old Qt-less
   environment “current”. It now identifies the original 2026-11-03 environment.
4. **Local macOS SDK minimum mismatch.** The project defaults to macOS 12 for
   its CI Qt 6.8 SDK. Local Qt 6.11.2 libraries target macOS 13; the linker warned
   when using the default. Both build guides now require selecting a deployment
   target consistent with a newer SDK. The installer builder still targets the
   CI baseline; changing SDKs for shipped packages requires reviewing its OS floor.

## Validation performed

- AppleClang 21, Release, Qt-free core: 2/2 CTest tests passed.
- Full desktop build using local Qt 6.11.2: 4/4 CTest tests passed, including
  audio capture and offscreen GUI integration previews.
- Python packaging/licence regressions after the path fix: 8/8 tests passed.
- Markdown local file links and git whitespace checks performed.

## Scope and remaining work

Review covered the DSP/tuning and capture paths, settings/device selection,
GUI scaling/readouts, CMake, CI and packaging validation, plus text documentation.
No new pitch-detection failure was reproduced by these tests. This is not proof
of complete correctness or a comprehensive security/performance audit.

No live instrument/microphone test, Windows execution, Qt 6.5 build, installer
installation, signing/notarization or new ground-truth accuracy measurement was
performed locally. CI must validate the PR on its supported platforms. Existing
PDF manuals were retained; their page layout/content was not revalidated in this
review. The documentation index distinguishes current guides from historical
Hungarian records instead of translating or rewriting past evidence.

Existing release work remains open: full shipped-dependency notice/source audit,
broader hardware/permission testing, anti-aliasing/resampling improvements,
GUI-thread DSP latency and additional architectures. See ROADMAP.md and
RELEASE_CHECKLIST.md. No legal clearance or new platform support is claimed.
