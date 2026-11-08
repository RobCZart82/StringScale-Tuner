# StringScale Tuner 0.2.0-alpha.1 – kiadási jegyzet

Megjelent 2026-11-07-én. [Kiadás és letöltés](https://github.com/RobCZart82/StringScale-Tuner/releases/tag/v0.2.0-alpha.1).
A következő tervezett kiadás 1.0.0 stabil.

Ingyenes, önálló gitár- és basszushangoló Windows x64 és macOS Apple Silicon
rendszerekre. A felület felül jelszintmérőt, középen ±50 centes analóg mutatót,
alul élő hangmagasságskálát mutat húrjelölésekkel. A mért hang oktávszáma
és a célhúr külön látszik; Hz-értékek nincsenek a felületen.

- Standard gitár, Drop D és további gitár-/basszusprofilok.
- Bemeneti eszköz és csatorna választása, eszközönként megjegyzett csatorna.
- Magyar/angol nyelv, kékes törtfehér világos és antracit sötét téma.
- Arányos 75/100/125/150% fix felületméretek, natív Windows/Mac ikonok.
- Elkülönülő párbeszédablakok és sík lefelé mutató listajelölők.
- Rövid csomagszünetek és bizonytalan becslések miatti villogás javítása.
- Névjegy eredeti vektoros logóval és szignóval.

## Kipróbált konfigurációk

Robert a Mac mini / SSL 2+ és Windows 10 / Behringer UCG-102 konfigurációkon
sikeresen ellenőrizte a hat standard húrt és a ±25–40 centes eltéréseket.
A részletek és korlátok a repository docs/HARDWARE_TEST_RESULTS_HU.md
dokumentumában szerepelnek; a felhasználói telepítő nem mellékel tesztjegyzőkönyvet.

## Indítás és korlátok

Windows x64 EXE és macOS Apple Silicon PKG próbatelepitő is készül.
Qt/CMake/Visual Studio telepítése a felhasználónál nem szükséges.
A külön developer ZIP kicsomagolható fejlesztői csomag.
A mikrofonhozzáférést engedélyezni kell. Nincs kiadói aláírás/notarizáció;
a Mac-csomag ellenőrzött technikai ad-hoc aláírást kap. Az operációs rendszer
figyelmeztethet. A telepített rövid README és a két PDF-kézikönyv ismerteti az indítást.

A többi hangszer-/hangolási profil valós hardveres tesztelése részleges.
VST3, Linux, Intel Mac és Windows ARM nem része ennek a kiadásnak.
A közzétett kiadásban a telepítők, manifestek, SHA256-fájlok és a Qt/FFmpeg
forráscsomag elérhetők. A telepítők a függőségi licencértesítéseket is tartalmazzák.

## Újabb ellenőrzés

A túlvezérlés javítása után a program friss, tiszta hangmintákra vár, mielőtt
újra hangmagasságot jelez. 2026. november 6-án Robert macOS-en és Windowson
is sikeres működésről számolt be. A pontos build és rendszerkonfiguráció
ehhez a próbához nincs megadva. A november 7-i publikált build
Windows 10 Intel és macOS Tahoe M1 próbáját a hardveres jegyzőkönyv rögzíti.

[English release notes](https://github.com/RobCZart82/StringScale-Tuner/blob/main/docs/RELEASE_NOTES.md)

A publikált alpha forrása `4932abdd12c11545d92dd12ba3ab7b02e087cfca`, build 84.
Az [1.0.0 feltételeit](https://github.com/RobCZart82/StringScale-Tuner/blob/main/docs/RELEASE_1_0_AUDIT_HU.md) külön lista rögzíti.
