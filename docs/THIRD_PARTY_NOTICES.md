# Third-party component information – preview

StringScale Tuner's original source code is covered by the included MIT LICENSE.
The bundled frameworks, plugins and runtime libraries have separate licences.
The licenses directory includes the full LGPL v3, GPL v3 companion and LGPL v2.1
texts, with upstream provenance and SHA256 records. Their presence alone does not
complete the component-specific review.

This file is an inventory guide for the test build, not a completed public-release
licence bundle or a statement that all third-party obligations have been cleared.

- Qt 6.8.3: Qt Core, Gui, Widgets, Network, Multimedia and SVG are used directly
  or transitively. Their platform plugins may also be deployed.
  Licensing: https://doc.qt.io/qt-6.8/licensing.html
  Third-party components: https://doc.qt.io/qt-6.8/licenses-used-in-qt.html
  Source archive: https://download.qt.io/archive/qt/6.8/6.8.3/single/
- Qt Multimedia deployment may include FFmpeg libraries. The exact deployed
  files must be checked against file-inventory.json and the supplier's notices.
- Windows packages also contain the required MSVC runtime files. They are not
  covered by the application's MIT licence.

The qt-sdk-sbom directory, when available, contains supplier-provided SDK SBOM
files for audit. These describe the installed Qt SDK and can list components
that are not actually shipped in this application's archive. They do not replace
the applicable licence texts, copyright notices or source-availability review.

Before the first public release, finish the audit of the actual binaries,
include the required notices/licence texts and confirm any source-distribution
requirements. Public release publication is a separate step from these test builds.

The developer ZIP also includes runtime-licence-audit.json: actual deployed binary
paths, hashes, provisional component categories and explicit pending checks.
This audit data is intentionally not installed with the user documents. In
particular, an FFmpeg library filename does not establish its exact source version
or build options. Unknown components stay marked unclassified rather than cleared.

## Runtime inventory checkpoint

The [Desktop preview build 78](https://github.com/RobCZart82/StringScale-Tuner/actions/runs/42161042805)
for revision `6b0b61a997c78ed0b387dc39541ceb980b6f1a03` passed both
installer smoke jobs. Both downloaded installers matched their SHA256 files
and manifests. The installer audit inventories for this run contain 82
macOS arm64 and 77 Windows x64 payload entries, including documents and links.

Both inventories contain FFmpeg codec and format libraries with major version
61 in their filenames. The Windows inventory also lists MSVC runtime DLLs,
`D3Dcompiler_47.dll` and `opengl32sw.dll`. The next audit step is to match those
actual binaries to their supplier versions, notices and source records where
applicable, alongside the Qt components. Repeat the inventory review for the
final release candidate if its payload differs.
