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

### FFmpeg source evidence for build 78

The `av_version_info` and `avutil_license` exports identify FFmpeg **7.1**
and **LGPL-2.1-or-later** on both platforms. The macOS values were read from
the library expanded from the PKG. The Windows values were read from the
export targets in the DLL from the developer ZIP and matched to the installer
payload inventory by SHA256. The recorded `avutil_configuration` strings
select shared libraries and contain neither `--enable-gpl` nor
`--enable-nonfree`.

| Inspected binary | SHA256 |
| --- | --- |
| macOS `libavutil.59.dylib` | `99083a3015af89d7e155a7365cb6a63d2e603cd5cded30eece7429d34ec4c140` |
| Windows `avutil-59.dll` | `2ec506ddd253d3366dfc223d5bb62a30488700b966541287c499764c0a8df782` |

The Qt 6.8.3 provisioning scripts select the same FFmpeg `n7.1` source
archive for [macOS](https://github.com/qt/qt5/blob/v6.8.3/coin/provisioning/common/unix/install-ffmpeg.sh)
and [Windows](https://github.com/qt/qt5/blob/v6.8.3/coin/provisioning/common/windows/install-ffmpeg.ps1).
The downloaded [source archive](https://github.com/FFmpeg/FFmpeg/archive/refs/tags/n7.1.tar.gz)
matched the scripts' SHA-1 `f008a93710a7577e3f85a90f4b632cc615164712`.
Its SHA256 is `7ddad2d992bd250a6c56053c26029f7e728bebf0f37f80cf3f8a0e6ec706431a`.
Retain the matching source archive, configuration and supplier build instructions
with the final release materials.

Common observed configuration:

```text
--disable-programs --disable-doc --disable-debug --enable-network
--disable-lzma --enable-pic --disable-vulkan --disable-v4l2-m2m
--disable-decoder=truemotion1 --enable-shared --disable-static
```

The Windows DLL also reports `--prefix=installed --toolchain=msvc`.
The macOS arm64 slice also reports
`--extra-ldflags='-Wl,-ld_classic' --prefix=/usr/local/FFmpeg-n7.1
--enable-cross-compile --arch=arm64 --cc='clang -arch arm64'`.
The macOS library also has an x86_64 slice with corresponding x86_64 flags.

### Remaining work for public distribution

- Collect copyright notices and licence texts for the Qt modules and embedded
  third-party components present in the final payload. Review the versioned Qt
  source and SDK SBOM together. Arrange project-controlled corresponding-source
  access and verify that the packaged shared libraries can be replaced.
  See [Qt licensing](https://doc.qt.io/qt-6.8/licensing.html) and
  [LGPL obligations](https://www.qt.io/development/open-source-lgpl-obligations).
- Attach the verified FFmpeg source material for the final payload and collect its
  component notices. Repeat the binary-to-source check after a Qt or FFmpeg update.
  See [FFmpeg's distribution checklist](https://ffmpeg.org/legal.html).
- Review the shipped MSVC DLLs and `D3Dcompiler_47.dll` under the applicable
  Microsoft redistribution terms. The [Visual Studio redistribution list](https://learn.microsoft.com/en-us/visualstudio/releases/2022/redistribution)
  is a starting reference for files from that product.
- Treat `opengl32sw.dll` separately. The inspected DLL identifies Mesa 11.2.2,
  consistent with the [Qt software renderer build notes](https://wiki.qt.io/MesaLlvmpipe).
  Match the binary to its supplier distribution and collect the Mesa/LLVM
  notices, or verify and document a candidate that omits the optional renderer.

The current repository licence remains MIT for the project's original code.
Keep the dependency review open until the final packages and accompanying
source and notice materials have been checked together.
