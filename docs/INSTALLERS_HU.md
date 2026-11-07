# Natív próbatelepitők

A Desktop preview workflow a ZIP mellett Windows x64 EXE és macOS Apple Silicon
PKG telepítőt is készít. Ezek tesztelői artifactok, nem automatikus GitHub Release-ek.
A függőségi licencaudit teljesítése továbbra is nyilvános kiadási feltétel.

## Windows

Inno Setup 6 telepítő, magyar és angol varázslóval. Alapesetben a bejelentkezett
felhasználó LocalAppData/Programs/StringScaleTuner mappájába kerül, rendszergazdai
jog kérése nélkül. Start menü alkalmazás- és kézikönyv-hivatkozásokat, opcionális
asztali ikont és rendszerből elérhető eltávolítót ad. A beállításokat az app menti;
az eltávolító a felhasználó hangolási/beállítási adatait nem törli külön.

A tesztelő CI csendes telepítést, az összes telepített fájl ellenőrzését, a
telepített alkalmazás mikrofon nélküli előnézeti indítását és eltávolítást végez.
Az EXE nincs kiadói tanúsítvánnyal aláírva; SmartScreen figyelmeztethet.

## macOS

A pkgbuild/productbuild által készített, aláíratlan PKG a rendszer Applications
mappájába telepíti a StringScaleTuner.app alkalmazást és a „StringScale Tuner
Documents” mappát. Utóbbiban csak a két PDF-kézikönyv, rövid README és licencanyag
található. A fejlesztői buildadatok nem kerülnek a telepített mappába. Rendszerszintű telepítéskor a macOS hitelesítést kérhet.

A PKG nem relocatable: egy másik mappában maradt tesztpéldány helyett az
Applications útvonalat használja. Telepítés előtt zárd be a régi alkalmazást.
A dokumentáció és az app újabb próbacsomaggal ugyanarra a helyre frissíthető.
Az alkalmazás ad-hoc aláírását megőrzi; a telepítő nincs Developer ID Installer
aláírással ellátva és nincs notarizálva. A kézi engedélyezés a kézikönyvben van.

A csomagolás kibontja és fájlonként ellenőrzi a PKG payloadját. A GitHub-hosted
Mac VM ezen felül valóban telepíti is, majd ellenőrzi és mikrofon nélkül indítja.
A fejlesztő vagy Robert saját Macjén ez a folyamat nem indít automatikus telepítést.
Macen nincs külön eltávolító: az app és a fenti dokumentációs mappa a Finderből
eltávolítható. A személyes beállítások ettől megmaradhatnak.

## Azonosítás

A fájlnév verziót, architektúrát és buildszámot tartalmaz. A telepítő mellé
SHA256 és JSON manifest kerül. A ZIP-forrás és telepítő ugyanazt a forrásazonosítót
használja. A technikai aláírás és ellenőrzőösszeg nem jelent kiadói hitelesítést.

A Windows fordításhoz Inno Setup 6 kell (a Windows Actions-image tartalmazza).
A Mac-fordításhoz az Xcode Command Line Tools pkgbuild/productbuild/pkgutil kell.

## Rendszerkövetelmények

A csomagok Qt 6.8.3-at használnak. A telepítő küszöbe Windows 10 1809 (17763)
és macOS 12.0 / Apple Silicon. A Mac deployment target és az Info.plist is 12.0;
újabb Qt SDK használatakor a [fordítási útmutató](BUILD_HU.md#újabb-qt-és-macos-minimum)
szerint állítsd be a helyi fordítás célverzióját. A PKG külön minimumot állít be.
Ezek a Qt-alapú technikai minimumok, nem minden OS-verzió hardveres tanúsítása.
Robert konkrét próbái a HARDWARE_TEST_RESULTS_HU.md dokumentumban vannak.

Hivatkozások:
- https://doc.qt.io/qt-6.8/supported-platforms.html
- https://jrsoftware.org/ishelp/topic_setup_privilegesrequired.htm
- https://developer.apple.com/documentation/xcode/packaging-mac-software-for-distribution

## Felhasználói dokumentációs szabály

Új telepítés dokumentációja: README.txt, LICENSE, THIRD_PARTY_NOTICES.md,
manuals/StringScale-Tuner-User-Manual-HU.pdf és -EN.pdf; szükség esetén licenses/
alatt a függőségek további jogi mellékletei. Az appban/binaryk mellett található,
futásidőhöz tartozó licencanyagokat nem töröljük. A manuals fejlesztői README-je
nem kerül a felhasználóhoz.

A tesztlapok, hardveres jegyzőkönyv, kiadási munkajegyzet, SDK-SBOM és JSON
build-/fájlleltár csak a repositoryban, a „developer” ZIP-ben vagy külön CI-audit
artifactban maradnak. A telepített fájlok ellenőrzése külső, CI-beli leltárból
megy, így nem kell ellenőrzési JSON-t telepíteni. A teljes licencaudit továbbra
is kiadási feltétel, a szűrés nem helyettesíti azt.

Korábban telepített próbaverzióból maradhatnak régi dokumentumok. Ezeket az új
telepítő nem törli automatikusan, nehogy kitöltött személyes tesztlap vesszen el.
A régi dokumentációs mappa mentés után kézzel eltávolítható; friss telepítéskor
már csak a fenti felhasználói anyagok kerülnek bele.

## Az aláíratlan próbaverzió telepítése

Az EXE vagy PKG fájlt a [hivatalos kiadásból](https://github.com/RobCZart82/StringScale-Tuner/releases/tag/v0.2.0-alpha.1)
töltsd le. Ez az alpha nem rendelkezik kiadói tanúsítvánnyal. Az alábbi
lépések a szándékosan letöltött fájl ismeretlen kiadójára vonatkozó
figyelmeztetésnél használhatók.

### Windows 10 / Windows 11

1. Nyisd meg a letöltött Windows x64 EXE-t.
2. Ha megjelenik a kék **A Windows megvédte a számítógépét** SmartScreen-ablak,
   válaszd a **További információ**, majd a **Futtatás mindenképpen** lehetőséget,
   ha elérhető. Angol rendszeren ezek **More info → Run anyway**.
3. Kövesd a telepítő lépéseit, válaszd ki a kívánt parancsikonokat, és fejezd be a telepítést.
4. Indítsd a **StringScale Tuner** alkalmazást a Start menüből vagy a létrehozott ikonról.

A telepítő alaphelyzetben az aktuális felhasználónak telepít. Ha a gép szabályzata
tiltja a futtatást, vagy a folytatási lehetőség hiányzik, kérd a rendszergazda segítségét.

### macOS Apple Silicon gépen

1. Nyisd meg a letöltött PKG-t. Ha a macOS az ismeretlen fejlesztő vagy
   az Apple-ellenőrzés hiánya miatt blokkolja, zárd be a figyelmeztetést.
2. Nyisd meg az **Apple menü → Rendszerbeállítások → Adatvédelem és biztonság**
   oldalt, és görgess a **Biztonság** részhez. Keresd meg a blokkolt telepítő bejegyzését.
3. Válaszd a **Megnyitás mindenképpen** lehetőséget, szükség esetén hitelesíts,
   majd erősítsd meg a megnyitást. Angol rendszeren a gomb **Open Anyway**.
   Ha eltűnt a lehetőség, próbáld újra megnyitni a PKG-t, majd térj vissza ide.
4. Kövesd a macOS Telepítő lépéseit, majd indítsd az **Alkalmazások** mappából
   a **StringScaleTuner** programot.
5. Ha maga az alkalmazás is azonos fejlesztői figyelmeztetést kap, ismételd meg
   rá a 2–3. lépést. Engedélyezd a mikrofonhozzáférést, külső interfész esetén is.

A feliratok rendszerverziónként és nyelvenként eltérhetnek. macOS 12 alatt a
beállításkezelő angol neve System Preferences. Felügyelt gépen a jóváhagyás korlátozott lehet.

### Más jellegű biztonsági riasztás

Kártevőriasztás, karantén vagy „sérült alkalmazás” üzenet esetén állj meg,
és tölts le friss példányt a hivatalos kiadásból. Ellenőrizd a mellékelt SHA256-ot,
és tartós probléma esetén jelezd a [hibajegyeknél](https://github.com/RobCZart82/StringScale-Tuner/issues).
A rendszerszintű védelem maradjon bekapcsolva. A fenti lépések egy kiválasztott
letöltés engedélyezésére szolgálnak, vírusirtó, SmartScreen vagy Gatekeeper kikapcsolása nélkül.

Források: [Apple megnyitási útmutató](https://support.apple.com/en-us/102445),
[Microsoft SmartScreen-tájékoztató](https://learn.microsoft.com/en-us/windows/apps/package-and-deploy/smartscreen-reputation).
