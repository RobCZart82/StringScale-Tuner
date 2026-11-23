# Első kiadás előtti ellenőrzőlista

Következő cél **1.0.0 stabil**. A következő kiadáshoz az
[1.0.0 elfogadási lista](RELEASE_1_0_AUDIT_HU.md) tartozik.
Az alábbi alpha-lista a korábbi kiadási folyamatot írja le.

## A stabil kiadásjelölt hardveres próbáinak rögzítése

Robert visszajelzése szerint az 1.0.0-rc.1 113-as build telepítője és alkalmazása működött Windows x64-en és macOS Apple Siliconon. A próba hatóköre a [hardveres naplóban](HARDWARE_TEST_RESULTS_HU.md#2026-11-23--100-rc1-113-as-build) olvasható.

A két telepítő ugyanabból a sikeres Desktop preview futásból származzon.
Próbánként rögzítsd a verziót, a teljes forráscommitot, a futás számát,
az operációs rendszer verzióját, a hanginterfészt, a csatornát és a hangszert.
Ellenőrizd a letöltött telepítőpárt a jelölt tényleges verziójával.

| Próba | Rögzítendő |
| --- | --- |
| Minden hirdetett hangolás, köztük az öthúros basszus B0 és a héthúros gitár B1 | Referenciahangoló vagy jelforrás, húr, várt és mért centérték, a stabil kijelzésig eltelt idő |
| Bemenet kihúzása és visszacsatlakoztatása hangolás közben | Törlődik-e a régi hang, milyen állapot jelenik meg, és kell-e újraválasztani a bemenetet |
| Mikrofonengedély megtagadása, majd visszaadása | Kijelzett állapot, a helyreállítás lépései és kellett-e újraindítani az alkalmazást |
| Frissítés a közzétett alpháról, majd eltávolítás | Telepítési eredmény, megmaradt beállítások és az alkalmazás eltávolításának eredménye |

A pontosság és a beállási idő elfogadási határait a mérés előtt rögzítsd.
Minden próbát jelölj sikeresnek, sikertelennek vagy el nem végzettnek.
A kimaradt próbákat és a hibák ismétlési lépéseit is jegyezd fel.
A jelölt eredményeit az [alpha korábbi eredményeitől](HARDWARE_TEST_RESULTS_HU.md)
külön rögzítsd.

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
