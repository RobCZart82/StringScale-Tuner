# Roadmap

- [x] M0: Original C++20 pitch-detector, tuning reference model, CI-core tests.
- [x] M0: Implement Qt vector widgets for analog gauge, horizontal pitch map,
      on-line red pitch dot, and input level meter (source code only).
- [x] M0: Audio capture integration draft and basic preset/settings workflow.
- [ ] M1: Compile and launch Qt GUI on Windows 10/11 and macOS (Intel/ARM).
- [ ] M1: Test USB DI interfaces; inspect sample formats, device switching,
      real microphone permission and dynamic unplug behavior.
- [ ] M2: Ground-truth pitch accuracy with guitar and bass at E2/E1/B0,
      detunings, transients, vibrato, noise and weak fundamentals.
- [ ] M2: Improve latency: DSP thread, adaptive analysis window, resampler.
- [ ] M2: Add user-selectable input channel for multichannel DI interfaces.
- [ ] M2: Add optional manual target-string selection and clearer clip hold.
- [ ] M3: Polish multi-resolution retina layouts, accessibility and localization.
- [ ] M3: GUI integration tests and platform build jobs.
- [ ] M4: Signed/notarized macOS .app/.pkg and Windows installer, open-source
      license notices and reproducible release workflow.
