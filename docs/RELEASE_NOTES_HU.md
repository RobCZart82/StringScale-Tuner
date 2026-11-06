# StringScale Tuner 0.2.0-alpha.1 – kiadási jegyzet tervezete

Állapot: kiadásra előkészített szöveg; még nem közzétett GitHub Release.

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
A teljes függőségi licencmelléklet és a végleges forrásállapot ellenőrzése még
kiadási feltétel; ez a jegyzet nem jelenti ezek teljesítését.

A kiadáshoz a végleges forráscommitot, ZIP-eket és SHA256-összegeket kell csatolni.

## Újabb ellenőrzés

A túlvezérlés javítása után a program friss, tiszta hangmintákra vár, mielőtt
újra hangmagasságot jelez. 2026. november 6-án Robert macOS-en és Windowson
is sikeres működésről számolt be. A pontos build és rendszerkonfiguráció
ehhez a próbához nincs megadva. A közzététel előtt a két telepítőt a végleges
forráscommithoz kell ellenőrizni a [kiadási eljárás](RELEASE_CHECKLIST.md) szerint.

[English release notes](RELEASE_NOTES.md)
