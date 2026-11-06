# Native test installers

Desktop preview builds Windows x64 EXE and macOS arm64 PKG artifacts alongside
developer ZIPs. These are test artifacts; no workflow publishes a GitHub Release.

Windows uses Inno Setup 6 with English/Hungarian wizard options. The default is
LocalAppData/Programs/StringScaleTuner for the current user, without administrator
access. Start menu links include the app/manuals; a desktop shortcut is optional.
Uninstall removes the application but does not explicitly erase saved settings.
CI installs silently, verifies installed files/icons, renders a microphone-free
preview and uninstalls. There is no publisher certificate; SmartScreen may warn.

macOS uses pkgbuild/productbuild to install StringScaleTuner.app and
StringScale Tuner Documents in /Applications. System installation may require
authentication. The app is ad-hoc signed after deployment; the PKG has no publisher
signature or Apple notarization. CI expands/verifies the payload, installs it on
a disposable runner and launches its preview. This does not test microphone permission
or establish Gatekeeper acceptance on an end user's machine.

The user documents contain both PDF manuals, a short README and licence notices;
SDK SBOMs, inventories and developer test sheets stay outside the installed documents.
The complete third-party licence/source audit remains a release requirement.
See the [Hungarian installer guide](INSTALLERS_HU.md) and [release checklist](RELEASE_CHECKLIST.md).

## System requirements

CI packages Qt 6.8.3. The Windows installer sets a floor of Windows 10 1809
(build 17763), and the macOS installer targets macOS 12.0 on Apple Silicon.
These are installer thresholds. See the [hardware report](HARDWARE_TEST_RESULTS_HU.md)
for configurations that were actually tried and the [build guide](BUILD.md)
for choosing a deployment target with a different Qt SDK.
