# Desktop preview

In GitHub Actions, open a successful **Desktop preview** run and download the
artifact matching Windows x64 or macOS arm64. GitHub sign-in may be required.
The developer artifact contains a versioned ZIP; extract both archive layers.
Keep runtime libraries beside the executable. Launch bin/StringScaleTuner.exe on
Windows or copy StringScaleTuner.app to Applications on macOS and allow microphone access.
No local Qt SDK or compiler is needed. Separate installer artifacts contain EXE/PKG packages.

The `.zip.sha256` checks the inner versioned ZIP, not GitHub's outer artifact ZIP.
Use `Get-FileHash <file.zip> -Algorithm SHA256` on Windows or
`shasum -a 256 <file.zip>` on macOS. Record the About version/build and the
build-info.json revision with any feedback. TEST-RESULTS-HU.md is a fill-in test
sheet, not evidence of a successful test. Synthetic previews use no live input.

Developer ZIPs contain manifests, audit data and test records. User installers
retain the application/runtime, two PDF manuals, short README and licence material.
Unsigned test packages are not a published, signed release. See
[installers](INSTALLERS.md), [hardware evidence](HARDWARE_TEST_RESULTS_HU.md) and
[the detailed Hungarian preview guide](PREVIEW_HU.md).
