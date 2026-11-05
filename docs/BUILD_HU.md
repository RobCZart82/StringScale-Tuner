# StringScale Tuner - fordítás és első indítás

Ez fejlesztői buildleírás. A verzió egyetlen forrása a CMakeLists.txt;
a Névjegy és a csomagok generált buildadatai ezt követik.
Felhasználóknak nem szükséges fejlesztőkörnyezet: az EXE/PKG próbatelepitők és
a kézikönyvek használata az [installer-útmutatóban](INSTALLERS_HU.md) szerepel.
A forrás-ZIP, a developer ZIP és a telepítő különböző csomagok.

## Szükséges fejlesztőeszközök

- C++20 fordító és CMake 3.21+.
- Qt 6.5+ Core/Gui/Widgets/Multimedia/Svg modulokkal; a CI Qt 6.8.3-at használ.
- Windows x64: Visual Studio 2022 C++ build tools vagy megfelelő Qt Creator kit.
- macOS Apple Silicon: Xcode parancssori eszközök és Qt macOS kit.
- Natív telepítő építéséhez Windowson Inno Setup 6, Macen pkgbuild/productbuild.
- Python 3 a csomagoló és csomagellenőrző szkriptekhez.

## Windows x64

```powershell
cmake -S . -B build -A x64 -DCMAKE_PREFIX_PATH="C:/Qt/6.8.3/msvc2022_64"
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
cmake --install build --config Release --prefix stage
python -m unittest discover -s tests -p installer_payload_test.py
```

A generátortól függően a futtatható fájl például build/Release alatt készül.
A cmake --install a Qt deployment scriptjét is futtatja; a stage tartalmazza a
terjesztéshez szükséges Qt-függőségeket. Ha közvetlenül a buildből indítasz,
Qt Creator vagy megfelelő fejlesztői környezet kellhet a DLL-ek megtalálásához.

## macOS Apple Silicon

```bash
xcode-select --install
cmake -S . -B build -DCMAKE_PREFIX_PATH="/path/to/Qt/6.8.3/macos" -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES=arm64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
cmake --install build --prefix stage
python3 -m unittest discover -s tests -p installer_payload_test.py
```

Az app build/StringScaleTuner.app alatt készül. A projekt alapértelmezett macOS
deployment targetje 12.0; ez nem minden rendszerverzió hardveres tanúsítása.
A stage elkészítése a Qt frameworköket is bemásolja. A végső technikai aláírást
és ellenőrzést a package_preview.py végzi a deploy után. Az alkalmazás
mikrofonhasználati leírást tartalmaz, a hozzáférést a rendszerben engedélyezni kell.

## Qt nélküli motortesztek

```bash
cmake -S . -B build-core -DSTRINGS_BUILD_GUI=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-core --config Release
ctest --test-dir build-core -C Release --output-on-failure
```

Ez a konfiguráció nem fordítja a GUI-t vagy az AudioCapture adaptert.
A teljes GUI/capture/telepítő ellenőrzést a desktop konfiguráció és CI adja.
A workflow az aktuális csomagolási parancsok forrása:
[desktop-preview.yml](../.github/workflows/desktop-preview.yml).

## Első kézi próba

1. A felső hangszerlistában válaszd ki a hangolási profilt.
2. A bemeneti listában válaszd a mikrofont vagy USB-interfészt.
3. A fogaskerék Hang pontjában válaszd a gitár fizikai bemenetéhez tartozó csatornát.
4. Egyszerre egy húrt pengess. Felül a jelszint, középen az analóg mutató,
   alul a húrskálán mozgó türkiz/zöld pont reagál.
5. A mért hang oktávszámát és a célhúrt külön olvasd. A cél a legközelebbi
   referenciahang, nem a fizikai húr biztos azonosítása.
6. A Beállítások a nyelvet, témát és fix felületméretet kezeli; a Hang ablak
   a bemeneti csatornát és jelszintküszöböket. Hz-értékek nem jelennek meg a GUI-n.

A már elvégzett hardveres próbákat és korlátaikat a
[tesztjegyzőkönyv](HARDWARE_TEST_RESULTS_HU.md) rögzíti.

## Újabb Qt és macOS minimum

A 12.0 alapérték a CI Qt 6.8 futtatókörnyezetéhez tartozik. Újabb SDK-nál
a `-DCMAKE_OSX_DEPLOYMENT_TARGET` értéke legalább az SDK minimuma legyen
(a helyben próbált Qt 6.11.2 esetén 13.0). Az újabb Qt-val sikeres fordítás
nem igazolja a macOS 12 kompatibilitást.
