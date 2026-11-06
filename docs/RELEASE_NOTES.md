# StringScale Tuner 0.2.0-alpha.1

[Magyar](RELEASE_NOTES_HU.md)

First public preview draft for Windows x64 and macOS on Apple Silicon.

StringScale Tuner is a free standalone guitar and bass tuner. It shows input
level, an analog ±50-cent needle, and a pitch scale with string numbers. The
measured note and octave appear separately from the target string.

- Guitar and bass presets include Standard, Drop D, E-flat, Open G and DADGAD.
- Select an input device and channel. The app remembers the channel for each device.
- Choose English or Hungarian, light or dark appearance, and one of four interface sizes.
- After digital clipping, the tuner waits for a fresh window of clean audio
  before showing a pitch again.

## Installation

Choose the Windows EXE or macOS PKG for your platform. See the
[installer guide](INSTALLERS.md) and the [English or Hungarian manual](manuals/README.md).
Allow microphone access if prompted. The Windows installer has no publisher
signature. The macOS app is ad-hoc signed and the package is unsigned and
not notarized. The operating system may display a security prompt.

## Validation and known limits

On November 6, Robert reported successful use on macOS and Windows after the
clipping fix. The exact build and system configurations for this round were
not supplied. Earlier, configuration-specific trials are recorded in the
[hardware results](HARDWARE_TEST_RESULTS_HU.md). Bass, alternate tunings and
additional devices still need broader hardware coverage. This preview targets
Windows x64 and macOS arm64. VST3 and other platform builds remain planned work.

Before publication, fill in the tested source revision and attach the verified
installer files, SHA256 checksums and manifests using the
[release procedure](RELEASE_CHECKLIST.md).
