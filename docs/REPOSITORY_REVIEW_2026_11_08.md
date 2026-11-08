# Repository review after the first stable-release fixes

Baseline `f5b517c2816ed34dbee92779a1368646df9a4a35`, reviewed 2026-11-08.
The published release is 0.2.0-alpha.1; the next target is stable 1.0.0.

## Findings

| Item | Priority | Evidence | Next action |
| --- | --- | --- | --- |
| A6 — unbounded analysis-frame counter | Medium | `AudioCapture::acceptSample` increments a signed int. While the input level remains below the gate, estimation is skipped and the counter does not reset. At 12,000 analysis frames/s, INT_MAX is reached in about 49.7 hours. An accelerated boundary probe setting the counter to INT_MAX and accepting four 48 kHz samples produced a UBSan signed-overflow report at `src/audio_capture.cpp:120`. | Saturate at the minimum frames required to estimate, or otherwise bound the counter. Add a regression test near the boundary. No 50-hour hardware session was run. |
| A4 — rejected upper endpoint | Low | A clean 4096-sample, 12 kHz sine at exactly 700 Hz is still rejected under 25–700 Hz bounds. Nearby 690 Hz is detected. | Define and test the supported endpoint policy. |
| A5 — unconstrained package symlinks | Medium hardening | A temporary installer-stage fixture with `licenses/escape` pointing to an absolute file outside the stage is accepted by `create_user_stage`. The preview validator checks target equality without containment. | Confine resolved targets and chains while retaining valid macOS framework links. No exploit of published CI artifacts was demonstrated. |
| Development log missing recent work | Documentation | The previous newest entry was November 5. It omitted PRs 25–31, the November 7 public release, hardware feedback, unsigned-install guidance, the 1.0.0 audit and its first fixes. | Add dated entries with source/build identifiers and scope. Updated in this review's documentation change. |
| Audit status requires a current summary | Documentation | The original A1–A3 reproductions remain at the start of the 1.0 report, with the fix follow-up only at the end. | Add a prominent current-status table and link this review. Updated in this documentation change. |

## Verification of earlier fixes

PR 31 is merged into the baseline. The existing capture suite includes the A1
alias rejection, A2 sparse input expiry and A3 invalid-settings regressions and
passes on this baseline. The architecture page now correctly describes the
eighth-order filter. These findings are recorded as implemented fixes, with
real-instrument accuracy and latency acceptance still required for 1.0.0.

## Checks performed

- Full Release GUI build with AppleClang 21, Qt 6.11.2 and macOS deployment
  target 13.0. Four CTest cases passed.
- Nine Python tests passed. All Python script/test sources passed syntax parsing.
- Local documentation file links were checked, including nested manual indexes.
- Reviewed DSP/capture, settings/device selection, UI scaling/readouts,
  CMake/workflows, packaging/provenance/source-bundle code and current text docs.
- Reproduced A4 with a core probe and A5 using a temporary stage.
- Built the capture test with UndefinedBehaviorSanitizer and a temporary A6
  boundary probe. The sanitizer reported signed overflow. Probe edits were
  removed from the production test file afterward.
- A PCM-format normalization probe did not demonstrate the suspected missing
  positive-rail clip warning, so it is not reported as a bug.

No syntax/build failure was found in the checked environment. This review does
not establish the absence of other defects. No new Windows execution, live
instrument run, 50-hour soak, full fuzz/performance review or PDF layout review
was performed. The specific UBSan probe is not a claim that the entire suite
is sanitizer-clean.

## Remaining documentation and release work

The READMEs identify the published alpha, and the project status records the
published source/build and the intended 1.0.0. The 0.2.0-alpha.1 values in the
current application, packaged README and two PDF manuals still identify the
current version. Refresh them together when preparing 1.0.0. Historical logs
and tagged source should retain their original dates and versions.

The release checklists retain the alpha process as history and link the stable
acceptance plan. The generated runtime audit still emits generic pending labels
and release_cleared=false. A final per-candidate evidence record is needed before
the stable release. Signing/notarization is a distribution-policy decision,
rather than a claim implied by the word stable.

See [the stable acceptance plan](RELEASE_1_0_AUDIT.md) and
[Hungarian summary](RELEASE_1_0_AUDIT_HU.md).
