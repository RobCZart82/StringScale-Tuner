# Application icon

The shared vector master app-icon.svg depicts a winged guitar with a tuning
fork forming its neck. A thin navy gap separates the cream guitar body from
the wings. The rounded navy background has transparent corners.

- PNG: 16, 20, 24, 32, 40, 48, 64, 128, 256, 512 and 1024 pixels.
- ICO: 16-256 pixel entries for the Windows EXE, shortcuts and setup icon.
- ICNS: standard 16/32/128/256/512 point slots with 1x/2x representations.
Regeneration on macOS (development only):

    python3 scripts/generate_app_icons.py --qt-root /path/to/Qt/6.8.3/macos

Requires Pillow, a Qt SDK and Apple command-line tools. Normal app builds use
the committed assets and do not need icon-generation dependencies.

The Qt application icon, Windows native resource and Inno Setup icon reference
the same artwork. The macOS bundle includes the ICNS and CFBundleIconFile entry.
The installer smoke test checks the installed native icon on both platforms.
