# Architecture notes — v0.1.0

## Signal pipeline

1. `QMediaDevices` lists microphones and audio interfaces.
2. `QAudioSource` captures an available normalized sample format (48k/multichannel
   preferred; device preferred format as fallback).
3. The input read handler calculates block RMS and peak, and monitors clipping.
4. A simple 1-pole low-pass and phase accumulator decimate to 12,000 sps.
5. 4096-sample rolling analysis window (~341 ms of signal at 12k) uses an
   original implementation of YIN's difference / cumulative mean normalized
   difference approach with parabolic refinement.
6. An amplitude gate and detector confidence gate suppress weak/ambiguous data.
7. The GUI receives absolute frequency and interpolated fine-tuning deviation.

The GUI timer runs at ~65 ms intervals. Effective response time is influenced
by the 341-ms analysis window, capture buffering, and GUI-thread processing.
This is a reference implementation, NOT a real-time guarantee. High priorities
for the next revision: move DSP off UI thread, more sophisticated band-limited
resampling, adaptive window length / harmonic rejection, and hardware testing.

## Measurements

- Pitch: absolute estimated fundamental frequency (not a frequency peak).
- Reference strings: fixed 12-TET frequencies, scaled by A4 calibration / 440.
- Red dot x-position: linear in log2(frequency), plotted directly on scale line.
  Plot range extends one octave below and above the lowest/highest selected
  string, so octave mistakes are visible rather than silently hidden.
- Analog meter: shown *only* for |detected/reference cents| <= 50. A note one
  octave away must never display a convincing '0 cents' gauge.
- RMS input gate: adjustable threshold, default -45 dBFS.
- Peak warning: default -3 dBFS; true clipping flagged close to full scale.

## Scope and licensing

All pitch detector, reference-tuning model, and Qt widget drawing code in this
repository is new code written for this project. No APTuner source code is used.
Qt is an external framework; distributed apps will need Qt license compliance.

## Design rationale

The pitch-map is not merely a chromatic note indicator. Its primary role is
to help novice guitarists understand whether their *absolute* pitch is below,
near or above their intended string. A visual 'in tune' indication must not
conceal an octave error. This is a key acceptance criterion.


Input channel selection (2026-11-04): the capture adapter negotiates an available
multichannel format and routes one selected channel to both level measurement
and pitch analysis. Channel selection is saved per device. The Qt-independent
input_frame.hpp helper rejects invalid/nonfinite selected samples; unrelated
channels are not mixed or included in clip detection. Changing the channel
clears analysis history while preserving partial PCM-frame alignment.
