# StringScale Tuner – fordítás és első indítás (HU)

**Fejlesztői változat, v0.1.0.** A mellékelt ZIP forráskódot tartalmaz,
nem kész EXE-t vagy PKG-t. A GUI-t és az élő hangbemenetet még nem sikerült
Windows/macOS hardveren kipróbálni; ezért a prototípus mérési pontossága
nem garantált.

## Szükséges szoftverek

- CMake 3.21 vagy újabb.
- Qt 6.5+ azonos platformhoz fordított csomagja, **Qt Widgets** és
  **Qt Multimedia** modullal.
- Windows 10/11: Visual Studio 2022 C++ build tools (MSVC), vagy Qt Creator
  kompatibilis fejlesztői kittel.
- macOS: Xcode parancssori eszközök és Qt clang/macOS kit.

## Windows (példa; a saját Qt útvonaladhoz igazítsd)

```powershell
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.8.3/msvc2022_64"
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

A `StringScaleTuner.exe` a build/Release környékén keletkezik a választott
CMake generátortól függően. Ha futtatásnál Qt DLL vagy multimédia-plugin
hiányzik, Qt Creatorból indítsd a programot; önálló EXE-terjesztéshez később
`windeployqt` és telepítő készítése szükséges.

## macOS (példa; a saját Qt útvonaladhoz igazítsd)

```bash
xcode-select --install
cmake -S . -B build -DCMAKE_PREFIX_PATH="$HOME/Qt/6.8.3/macos" -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
open build/StringScaleTuner.app
```

Engedélyezd a mikrofonhoz / audióbemenethez való hozzáférést a macOS
adatvédelmi beállításaiban. A macOS app tartalmaz `NSMicrophoneUsageDescription`
kulcsot. Ha futtatásnál Qt framework hiányzik, a valódi kiadáshoz
`macdeployqt` és aláírás/notarizálás szükséges.

## Qt nélküli DSP-teszt

A hangfelismerés és a referenciahangok tesztelhetők az előző fejlesztőkörnyezet
telepítése nélkül, ha van C++20 fordító és CMake:

```bash
cmake -S . -B build-core -DSTRINGS_BUILD_GUI=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-core --config Release
ctest --test-dir build-core -C Release --output-on-failure
```

## Első próba

1. A **Tuning** menüből válaszd ki a gitár/basszus hangolását.
2. Az **Input** listában jelöld ki a mikrofont vagy USB audiointerfészt.
3. Az **A4** kalibráció alapértéke 440 Hz.
4. Pengetésre az alsó sávban a szintjelző, a középső vízszintes skálán
   a **piros pont**, és célhang közelében a felső analóg mutató reagál.
5. A **Settings** alatt állítható a minimális RMS-jelszint és a magas
   csúcsszintre figyelmeztető határ.

**Biztonság:** Ha a mutató nem megbízható vagy nem jelenik meg, ellenőrizd a
jelszintet, az audioeszközt és a helyes oktávot. A túlfeszített húr elszakadhat;
a prototípust ne tekintsd hitelesített hangolóműszernek.
