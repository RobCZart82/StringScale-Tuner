# Release checklist

Target preview: 0.2.0-alpha.1, Windows x64 and macOS arm64. These are release gates,
not a claim that the current commit has passed CI or hardware validation.

- [ ] All required tests and both desktop builds pass for the final commit.
- [ ] Version, About, build metadata, source revision and package names agree.
- [ ] Downloaded inner ZIP matches its SHA256 checksum.
- [x] User-reported Mac/SSL 2+ and Windows 10/UCG-102 trials are recorded with limits.
- [ ] Explicit weak/overloaded input and microphone-denial trials are complete.
- [ ] Release-blocking defects are fixed and retested.
- [ ] Actual shipped dependencies have complete notices/licences/source review.
- [ ] Supported OS versions and architectures are validated and documented.
- [x] Ad-hoc signing, missing publisher signatures and notarization status are documented.
- [ ] Final commit, tag, release notes, known limitations and package checksums are fixed.
- [ ] Approved packages are attached to the GitHub Release.

Additional platforms, VST3 and publisher-signed/notarized packages are separate
milestones. Detailed historical checkmarks: [Hungarian checklist](RELEASE_CHECKLIST_HU.md).
