# Application icon

The shared vector master app-icon.svg is an original, simplified miniature of
StringScale Tuner's dark three-meter interface. It contains no screenshot text
or frequency numbers. The rounded-square silhouette has transparent corners.

- PNG: 16, 20, 24, 32, 40, 48, 64, 128, 256, 512 and 1024 pixels.
- ICO: 16-256 pixel entries for the Windows EXE, shortcuts and setup icon.
- ICNS: standard 16/32/128/256/512 point slots with 1x/2x representations.
- At 16-32 pixels the needle and arc are optically thickened and fine ticks
  omitted; the composition and palette stay the same.

Regeneration on macOS (development only):

    python3 scripts/generate_app_icons.py --qt-root /path/to/Qt/6.8.3/macos

Requires Pillow, a Qt SDK and Apple command-line tools. Normal app builds use
the committed assets and do not need icon-generation dependencies.

The Qt application icon, Windows native resource and Inno Setup icon reference
the same artwork. The macOS bundle includes the ICNS and CFBundleIconFile entry.
The installer smoke test checks the installed native icon on both platforms.
