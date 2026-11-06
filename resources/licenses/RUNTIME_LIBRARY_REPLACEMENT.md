# Using a modified runtime library

StringScale Tuner's Qt and FFmpeg runtime libraries are shared libraries. The
application source and build steps are in the repository. The release source
bundle contains the upstream source archives, hashes and notices. See the
project's [build guide](https://github.com/RobCZart82/StringScale-Tuner/blob/main/docs/BUILD.md)
and [Qt's source build guide](https://doc.qt.io/qt-6.8/build-sources.html).

Use a private copy of the installation when testing replacement libraries.
Build the modified library for the installed architecture and match the ABI
expected by the app and plugins. For a Qt change spanning several modules,
build and deploy those modules together. For FFmpeg, keep the shipped shared
library names and major versions and use the platform configuration described
in THIRD_PARTY_NOTICES.md.

- **Windows x64:** close the app, copy its installation directory, then replace
  the relevant DLLs in the copy's `bin` directory and any affected plugins in
  `plugins`. Launch the copied `bin/StringScaleTuner.exe`.
- **macOS arm64:** copy `StringScaleTuner.app` to a writable location. Replace
  the relevant frameworks or dylibs under `Contents/Frameworks` and any affected
  plugins under `Contents/PlugIns`, preserving framework layout, symlinks and
  relative library install names. Sign the modified copy ad-hoc with
  `codesign --force --deep --sign - /path/to/StringScaleTuner.app` and verify it
  with `codesign --verify --deep --strict /path/to/StringScaleTuner.app`. Launch
  the copy and grant microphone access if macOS prompts for it.

The original app source is MIT-licensed. The separate Qt and FFmpeg terms
govern the runtime libraries, including rights to modify the libraries and
debug those modifications.
