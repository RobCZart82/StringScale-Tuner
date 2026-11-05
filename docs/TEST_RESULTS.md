# Historical validation log - 2026-11-03

This is the original source-package test snapshot, not the current project status.
Subsequent desktop/installer CI and real-instrument feedback are documented in
[HARDWARE_TEST_RESULTS_HU.md](HARDWARE_TEST_RESULTS_HU.md) and
[PROJECT_STATUS_HU.txt](../PROJECT_STATUS_HU.txt).

The Qt-independent DSP engine was compiled with GNU C++ and with Clang using
AddressSanitizer and UndefinedBehaviorSanitizer (both passed). 

The deterministic unit test suite covers:

| Reference tone | Test type | Observed pitch error |
|---|---|---:|
| 30.8677 Hz (B0) | sine / harmonics | < 0.01 cent |
| 41.2034 Hz (E1) | sine / harmonics | < 0.01 cent |
| 73.4162 Hz (D2) | sine / harmonics | < 0.02 cent |
| 82.4069 Hz (E2) | sine / harmonics | < 0.03 cent |
| 110.0000 Hz (A2) | sine / harmonics | < 0.08 cent |
| 146.8324 Hz (D3) | sine / harmonics | < 0.10 cent |
| 196.0000 Hz (G3) | sine / harmonics | < 0.22 cent |
| 329.6276 Hz (E4) | sine / harmonics | < 0.42 cent |

The synthetic signals are stationary, clean, and generated at the same nominal
sample rate used by the detector. **These results do not establish equivalent
real-world accuracy** under plucks, noise, resampling, latency, harmonics with
missing fundamentals, or platform device variation. GUI and audio capture
were not compiled in the original 2026-11-03 validation environment because Qt SDK was unavailable.
