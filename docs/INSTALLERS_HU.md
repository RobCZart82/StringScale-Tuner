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
Documents” mappát. Utóbbiban találhatók a PDF-kézikönyvek, útmutatók, licencek és
buildadatok. Rendszerszintű telepítéskor a macOS hitelesítést kérhet.

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
a helyi új SDK ezért nem emeli automatikusan a szükséges macOS-verziót.
Ezek a Qt-alapú technikai minimumok, nem minden OS-verzió hardveres tanúsítása.
Robert konkrét próbái a HARDWARE_TEST_RESULTS_HU.md dokumentumban vannak.

Hivatkozások:
- https://doc.qt.io/qt-6.8/supported-platforms.html
- https://jrsoftware.org/ishelp/topic_setup_privilegesrequired.htm
- https://developer.apple.com/documentation/xcode/packaging-mac-software-for-distribution
