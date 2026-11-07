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

## Installing the unsigned preview

Download the EXE or PKG from the [official release](https://github.com/RobCZart82/StringScale-Tuner/releases/tag/v0.2.0-alpha.1).
This alpha has no publisher certificate. The following steps apply to an
unrecognized-publisher warning for the file you intended to download.

### Windows 10 / Windows 11

1. Open the downloaded Windows x64 EXE.
2. If the blue **Windows protected your PC** SmartScreen dialog appears,
   choose **More info**, then **Run anyway**, if offered.
3. Follow the installer, choose shortcut options if wanted, and finish installation.
4. Launch **StringScale Tuner** from the Start menu or the selected shortcut.

The installer normally installs for the current user. If an administrator
policy blocks it or the Run anyway option is absent, ask the administrator.

### macOS on Apple Silicon

1. Open the downloaded PKG. If macOS blocks it because the developer cannot
   be verified or Apple cannot check it, dismiss the warning.
2. Open **Apple menu → System Settings → Privacy & Security** and scroll to
   **Security**. Find the entry for the blocked installer.
3. Choose **Open Anyway**, authenticate if requested, and confirm opening.
   If this option has disappeared, try opening the PKG again and return here.
4. Complete the macOS Installer, then open **StringScaleTuner** in **Applications**.
5. If the app itself gets the same developer warning, repeat steps 2–3 for
   the app. Grant microphone access when asked, including for an external interface.

System wording varies with OS version and language. On macOS 12, the settings
application is named System Preferences. A managed Mac may restrict approval.

### A different security alert

If the system reports malware, quarantines the file, or says it is damaged,
stop and download a fresh copy from the official release. Check the supplied
SHA256 and report a persistent problem in the [issue tracker](https://github.com/RobCZart82/StringScale-Tuner/issues).
Keep system-wide protection enabled. These instructions approve one intended
download and do not require disabling antivirus, SmartScreen or Gatekeeper.

References: [Apple's opening instructions](https://support.apple.com/en-us/102445),
[Microsoft's SmartScreen guidance](https://learn.microsoft.com/en-us/windows/apps/package-and-deploy/smartscreen-reputation).
