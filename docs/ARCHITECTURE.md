# Architecture notes

Current source: the version is defined in CMakeLists.txt and generated into the
app's About dialog/build metadata. These notes describe the current implementation,
not the original source-package snapshot.

## Signal pipeline

1. QMediaDevices lists audio inputs. QAudioSource negotiates a multichannel
   format (48k Float/Int16 preferred, device preferred format as fallback).
2. Only the selected input channel feeds RMS, peak, clipping and pitch analysis.
   The channel is saved per device. Automatic fallback does not overwrite the
   preferred input device, which is restored when it reconnects. Explicit device
   changes update that preference, including before a permission prompt completes.
   Nonfinite selected samples are rejected;
   unrelated channels are not mixed into the measurement.
3. An eighth-order Butterworth low-pass with a 1.5 kHz cutoff precedes the
   phase-accumulator decimator to 12,000 samples/second. Four biquad sections
   suppress out-of-band tones that could otherwise alias into the 25–700 Hz
   analysis band. Coefficients are prepared when the capture rate changes.
   Filter history is cleared with the pitch window. The new-frame readiness
   counter saturates at 120 analysis frames, including during a closed level gate.
4. A 4096-sample rolling window (~341 ms) feeds the YIN-family difference/CMND
   estimator with parabolic refinement. Nonfinite bounds/samples and invalid
   ranges are rejected; lag limits are bounded by the input window before integer
   conversion. A dominant second harmonic is checked
   against a nearby doubled period. Correction requires an interpolated CMND
   mismatch reduction of at least 0.005 and at least 50 percent; interpolation
   reduces sample-grid bias. A Hann-windowed spectral check additionally requires
   a component at the proposed lower fundamental with at least 3 percent of its
   second harmonic's amplitude. This prevents rich, genuinely upper-octave tones
   from being lowered solely because their doubled-period mismatch is smaller.
   This reduces weak-fundamental octave errors without guaranteeing detection
   of arbitrarily weak or absent fundamentals. The adapter searches 25-700 Hz internally.
   A 5-cent endpoint fringe accommodates interpolation error; accepted fringe
   estimates are clamped to the requested interval. Lag search extends beyond
   the fringe to distinguish an actual nearby outside minimum from an edge.
5. The 65 ms GUI timer drains pending PCM and measures newly received samples.
   A packet-free tick does not mean digital silence: level is retained until
   a 250 ms data timeout, which clears analysis history.
6. A minimum-level gate uses 3 dB hysteresis. An uncertain estimate or short level
   dip retains the last valid pitch for at most 220 ms; failures cannot renew it.
   Ticks with too few new analysis frames also enforce that deadline.
   When a meter update reports digital clipping, it clears the pitch and
   discards the analysis window. Pitch resumes once a fresh window of clean
   samples is available. Stopping or changing channels also clears pitch.
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
  If even 75 percent does not fit, the outer window is bounded to the available
  screen and scrollbars expose the complete minimum-size instrument canvas.
  About and settings dialogs also retain a scaled content canvas inside a
  scrollable, screen-bounded window. Screen and work-area changes reapply their
  size limits; GUI previews exercise button reachability in a 260x180 work area.
- The app supports light/dark palettes, thin outlined popup selections and native
  ICO/ICNS application icons generated from the same original vector artwork.

## Packaging and verification

The desktop workflow builds Windows x64 and macOS arm64, runs CTest, deploys Qt
and produces a verified developer ZIP. Native installers consume a filtered copy:
app/runtime plus the two user manuals, short README and required licence material.
Test sheets, SDK SBOMs and build/file-inventory JSON stay in developer/audit output.

Package paths and symlinks are validated before archiving, on ZIP verification,
and before installer-stage copying. Resolved link targets must remain within the
package. Cycles, excessive chains and inventory entries beneath symlink parents
are rejected; normal macOS framework aliases are preserved.

The installer builder verifies staging against the archived inventory before
filtering. macOS packaging seals the final deployed app with ad-hoc signing;
the PKG payload is expanded and checked. CI actually installs/launches both
platform packages, checks installed files and native icons, and uninstalls Windows.
Publisher signing/notarization and the complete dependency licence audit remain
separate release work. No workflow automatically publishes a GitHub Release.

The original application code is MIT-licensed. Third-party components have their
own terms; see THIRD_PARTY_NOTICES.md and the release checklist.



Qt 6.5 remains the minimum supported API. Audio state/error enum types are
derived from QAudioSource so the QAudio-to-QtAudio namespace change does not
break this baseline. The microphone permission API is compiled only when the
Qt build enables its permissions feature. A dedicated Qt 6.5.3 CI job builds the application and runs
the regression tests alongside the packaged Qt 6.8.3 platform builds.
