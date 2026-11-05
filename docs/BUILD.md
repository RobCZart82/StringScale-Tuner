# Build and test

Version source: CMakeLists.txt. About and build metadata use its generated values.

Requires C++20, CMake 3.21+, Qt 6.5+ Core/Gui/Widgets/Multimedia/Svg.
CI packages Qt 6.8.3 and separately tests the Qt 6.5.3 API baseline.
Use MSVC 2022 on Windows or Xcode command-line tools on macOS.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/path/to/Qt/kit
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
python3 -m unittest discover -s tests -p '*_test.py'
cmake --install build --config Release --prefix stage
```

On Windows use `python` and select the x64 generator/Qt kit. To test without Qt,
configure with `-DSTRINGS_BUILD_GUI=OFF`. Python packaging tests run separately
from CTest. Core Linux support does not imply a validated Linux installer.

The Desktop preview workflow is the executable packaging recipe: install/deploy,
package_preview.py, verify_preview.py, build_installer.py and smoke_installer.py.
Native packages require Inno Setup 6 on Windows or pkgbuild/productbuild on macOS.
Icon regeneration additionally needs Pillow and Apple tools; ordinary builds use
committed icons. GUI previews use offscreen rendering without microphone input.

[Hungarian guide](BUILD_HU.md) · [Preview](PREVIEW.md) · [Installers](INSTALLERS.md)

The default macOS deployment target is 12.0 for the CI Qt 6.8 runtime.
With a newer Qt SDK, explicitly set `-DCMAKE_OSX_DEPLOYMENT_TARGET` to at least
the minimum supported by that SDK (13.0 for the locally tested Qt 6.11.2).
A successful build with newer libraries does not prove macOS 12 compatibility.
