# Roadmap

- [x] M0: Original C++20 pitch-detector, tuning reference model, CI-core tests.
- [x] M0: Implement Qt vector widgets for analog gauge, horizontal pitch map,
      on-line theme-colored pitch marker, and input level meter.
- [x] M0: Audio capture integration draft and basic preset/settings workflow.
- [x] M1: Windows x64 and macOS Apple Silicon GUI builds/launches in CI,
      with real Windows 10/Mac hardware feedback.
- [ ] M1: Additional Windows versions and Intel Mac builds/hardware validation.
- [x] M1: User-reported SSL 2+/UCG-102 input, channel and unplug/reconnect trials.
- [ ] M1: Broader device/sample-format coverage and explicit permission-denial tests.
- [ ] M2: Ground-truth pitch accuracy with guitar and bass at E2/E1/B0,
      detunings, transients, vibrato, noise and weak fundamentals.
- [ ] M2: Improve latency: DSP thread, adaptive analysis window, resampler.
- [x] M2: Add user-selectable input channel for multichannel DI interfaces
      (implementation, synthetic tests and documented user feedback).
- [ ] M2: Add optional manual target-string selection and clearer clip hold.
- [ ] M3: Polish multi-resolution retina layouts and accessibility.
- [x] M3: Add instant, persistent Magyar / English language selection in Settings
      alongside the light/dark theme selector; see the detailed requirements below.
- [x] M3: GUI integration tests and platform build jobs.
- [x] M3: Light/dark themes, fixed proportional size presets, native app icons.
- [x] M4: Unsigned Windows EXE/macOS PKG test installers with real CI install/launch checks.
- [ ] M4: Signed/notarized macOS .app/.pkg and Windows installer, open-source
      license notices and reproducible release workflow.


## Standalone first, VST3 extension

Accepted direction (2026-11-03): the free standalone tuner remains the primary
product. VST3 is a planned secondary format using the same C++ engine; no plugin
or additional platform compatibility is currently delivered.

Sequence: stabilize Windows x64/macOS arm64 standalone with real instruments;
retain the readable three-meter GUI without Hz labels; prepare the shared engine
for realtime hosting; prototype VST3; then validate Windows x64 and macOS Universal.
Intel standalone Mac support is also a goal. Windows ARM requires separate host
and SDK decisions (ARM64 versus ARM64EC). Linux x64 is a later experimental target;
Linux ARM is not a committed release target.

- [ ] Keep pitch detection, tuning and measurement state independent of Qt/VST3.
- [ ] Use preallocated buffers and a nonblocking, allocation-free audio callback;
      move analysis to another thread if needed, with safe GUI state transfer.
- [ ] Accept host audio in VST3, without opening devices or requesting microphone access.
- [ ] Pass audio through unchanged by default; offer an optional tuning mute.
- [ ] Validate sample rates, block sizes, channels, host lifecycle and plugin state.
- [ ] Prototype embedding the Qt GUI; no framework migration is mandated.
- [ ] Check the chosen SDK's licence and distribution requirements before release.

## About dialog and language requirements

The standalone About dialog and HU/EN switching are implemented and exercised by
GUI integration checks; broader display/accessibility hardware validation remains open.

The gear menu contains Settings, Audio and About. About uses the original GYR logo
and author signature as separate, proportionally scaled SVGs, respecting viewBox
origins. Dark/light variants retain transparent backgrounds. The personal name
appears only in the signature artwork; its accessible label is “Author signature”.
See the [asset notes](../resources/branding/README.md).

Settings offers independent appearance and language choices. Language options
remain “Magyar” and “English”. Changes apply immediately to the main window and
open dialogs and persist across restarts without changing capture or tuning.
Application labels, errors, tooltips and accessibility names are translated;
system device names are preserved. Notes, octaves and string numbering keep their
meaning; the GUI shows no Hz values. Long labels must fit in both themes/languages.
Without a saved preference, Hungarian systems start in Hungarian; all others in English.

Detailed accepted Hungarian requirements: [ROADMAP_HU.md](ROADMAP_HU.md).
