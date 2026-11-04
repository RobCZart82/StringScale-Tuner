# Architecture notes

Current source: the version is defined in CMakeLists.txt and generated into the
app's About dialog/build metadata. These notes describe the current implementation,
not the original source-package snapshot.

## Signal pipeline

1. QMediaDevices lists audio inputs. QAudioSource negotiates a multichannel
   format (48k Float/Int16 preferred, device preferred format as fallback).
2. Only the selected input channel feeds RMS, peak, clipping and pitch analysis.
   The channel is saved per device. Nonfinite selected samples are rejected;
   unrelated channels are not mixed into the measurement.
3. A one-pole low-pass and phase accumulator decimate to 12,000 samples/second.
4. A 4096-sample rolling window (~341 ms) feeds the YIN-family difference/CMND
   estimator with parabolic refinement. The adapter searches 25-700 Hz internally.
5. The 65 ms GUI timer drains pending PCM and measures newly received samples.
   A packet-free tick does not mean digital silence: level is retained until
   a 250 ms data timeout, which clears analysis history.
6. A minimum-level gate uses 3 dB hysteresis. An uncertain estimate or short level
   dip retains the last valid pitch for at most 220 ms; failures cannot renew it.
   Digital clipping, stopping or changing channels clears pitch immediately.
7. Nearby valid estimates are smoothed; a larger pitch change jumps to the new
   absolute pitch. Channel changes preserve partial PCM-frame alignment.

All reading and DSP currently run on the GUI thread. The rolling window,
capture buffering and processing affect response time; this is not a realtime
latency guarantee. DSP threading, improved resampling and adaptive windows remain
future work. Hardware results are recorded separately in HARDWARE_TEST_RESULTS_HU.md.

## Readout semantics

- The measured note includes its octave. No Hz readout is shown in the GUI.
- The target is the nearest string reference in the selected tuning preset,
  using absolute cents distance. Reference calibration is internal/legacy state;
  there is no public A4-frequency control in the current interface.
- The theme-colored circular marker is linear in log2(pitch) on the horizontal
  scale. Its range extends an octave below/above the selected reference range.
- The analog needle remains visible for a valid estimate and clamps at +/-50
  cents. Larger deviations get directional, out-of-range text; they are not hidden.
- The green in-tune message uses a +/-2 cent tolerance against the selected
  nearest reference. The measured note and target string are displayed separately.
- The app cannot infer which physical string the player intended to tune. A wrong
  octave can coincide with another valid reference; inspect both note/octave and
  target string number. Absolute display helps expose this, but is not a guarantee
  of zero octave errors in the detector or player input.

## UI and platform integration

- UiLanguage handles immediate/persistent HU/EN changes, including open dialogs.
- UiScale stores baseline fonts/layout metrics. Presets 75/100/125/150 percent
  scale widgets and painted meters together; 125 percent is the original
  1000x850 logical-pixel main window. Oversized presets are disabled for the screen.
- The app supports light/dark palettes, thin outlined popup selections and native
  ICO/ICNS application icons generated from the same original vector artwork.

## Packaging and verification

The desktop workflow builds Windows x64 and macOS arm64, runs CTest, deploys Qt
and produces a verified developer ZIP. Native installers consume a filtered copy:
app/runtime plus the two user manuals, short README and required licence material.
Test sheets, SDK SBOMs and build/file-inventory JSON stay in developer/audit output.

The installer builder verifies staging against the archived inventory before
filtering. macOS packaging seals the final deployed app with ad-hoc signing;
the PKG payload is expanded and checked. CI actually installs/launches both
platform packages, checks installed files and native icons, and uninstalls Windows.
Publisher signing/notarization and the complete dependency licence audit remain
separate release work. No workflow automatically publishes a GitHub Release.

The original application code is MIT-licensed. Third-party components have their
own terms; see THIRD_PARTY_NOTICES.md and the release checklist.
