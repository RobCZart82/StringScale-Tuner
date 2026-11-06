# Első kiadás előtti ellenőrzőlista

Tervezett első próbaverzió: 0.2.0-alpha.1, Windows x64 és macOS Apple Silicon.
Az alpha elnevezés nem igazolja a hardveres tesztek sikerét. Ez a lista nem
publikál automatikusan GitHub Release-t.

- [ ] A végső commit minden kötelező tesztje és mindkét asztali buildje sikeres.
- [ ] A csomag verziója, Névjegye és build-info.json adatai egyeznek.
- [ ] A SHA256 ellenőrzőösszeg a letöltött belső ZIP-re egyezik.
- [x] Robert Mac/SSL 2+ és Windows 10/UCG-102 hardveres visszajelzései
      rögzítve a HARDWARE_TEST_RESULTS_HU.md dokumentumban.
- [x] Hat standard húr, ±25/30/40 cent, nagy eltérés, oktáv- és csatornaváltási
      próbák felhasználói visszaigazolása rögzítve, hatókörrel és korlátokkal.
- [ ] Gyenge/túl erős bemenet és mikrofonengedély megtagadásának külön próbája.
- [x] Kihúzás/visszacsatlakoztatás, beállítások megőrzése, nyelv- és témaváltás
      sikeres felhasználói visszajelzése rögzítve.
- [ ] A próbák során talált kiadást akadályozó hibák javítva és újraellenőrizve.
- [ ] A ténylegesen csomagolt függőségek licenc- és forráselérhetőségi ellenőrzése
      kész; az előzetes THIRD_PARTY_NOTICES.md helyett teljes kiadási melléklet.
- [ ] A támogatott rendszerek és minimumverziók ellenőrizve, dokumentálva.
- [x] Ad-hoc aláírás/kiadói aláírás/notarizáció állapota és indítás dokumentálva.
- [ ] Rögzített forráscommit, verziócímke, kiadási jegyzet és ismert korlátozások.
- [ ] A jóváhagyott csomagok a GitHub Release-hez csatolva.

Az aláírt telepítő, további architektúrák, Linux és VST3 külön mérföldkő.
Egy első tesztelői alpha lehet kicsomagolható archívum; a stabil, széles körű
kiadás indítási és telepítési elvárásai ennél szigorúbbak.

## Történeti összevezetési megjegyzés

A #11 mainbe, a #12 a #11 ágába, a #13 pedig a #12 ágába olvadt.
Ezért a PR-ok „merged” állapota önmagában nem jelentette azt, hogy a main már
minden javítást tartalmaz. Az összevezető kiadás-előkészítő PR mindkét fejlesztési
ág történetét a mainhez kapcsolja. A végleges CI-t ezután ismét ellenőrizni kell.

A kész belső ZIP-et az upload előtt a scripts/verify_preview.py ellenőrzi:
SHA256, manifest/forrásazonosító, teljes fájlleltár, symlinkek és kötelező fájlok.
Ez nem helyettesíti a függőségek licencellenőrzését vagy a hardveres teszteket.

## A jelenlegi kiadásjelölt ellenőrzése

Az [angol kiadási eljárás](RELEASE_CHECKLIST.md) írja le a két telepítő
közös ellenőrzését. Azonos sikeres Actions-futásból töltsd le a Windows EXE-t,
a macOS PKG-t, valamint a mellettük lévő SHA256- és JSON-fájlokat. A
`scripts/verify_release_assets.py` ellenőrzi a letöltött fájlokat a megadott
verzió, teljes forráscommit és buildszám alapján. A 78-as build ellenőrzése
sikeres volt. A végleges kiadásjelöltön ezt meg kell ismételni.

A saját alkalmazáskód MIT-licencű. A csomagolt könyvtárakhoz a
[függőségi leltár](THIRD_PARTY_NOTICES.md) rögzíti a forrásadatokat és a
közzététel előtt lezárandó ellenőrzéseket.
