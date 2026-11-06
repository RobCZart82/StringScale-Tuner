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

## Verify the first public preview candidate

1. Choose a final `main` commit and wait for **Core DSP tests** and
   **Desktop preview** to succeed on that revision. Note the Desktop preview
   run number and check that both installer smoke jobs passed.
2. Download both `-installer` artifacts from that same run. Extract the EXE,
   PKG and their adjacent `.sha256` and `.json` files into one directory.
   Keep the developer and audit artifacts for dependency review.
3. Verify the six files using the expected version, full commit SHA and run
   number from GitHub Actions.

   ```sh
   python3 scripts/verify_release_assets.py /path/to/installers \
     --version 0.2.0-alpha.1 --revision FULL_COMMIT_SHA --build-number RUN_NUMBER
   ```

   Use these six files as the installer attachments. Keep the audit and
   developer artifacts separately for the dependency review.
4. Review the actual shipped runtime and finish the dependency notices and
   source-availability work. Track the remaining items in
   [Third-party notices](THIRD_PARTY_NOTICES.md). Prepare the verified source
   archives and licence materials alongside the installer attachments.
5. Once the gates above are satisfied, prepare a GitHub pre-release for the
   exact verified commit, using the [English release notes](RELEASE_NOTES.md)
   and linking the [Hungarian notes](RELEASE_NOTES_HU.md). Attach the six
   verified files, check the draft, then publish the pre-release.
