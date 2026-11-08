# Az 1.0.0 stabil kiadás előtti audit

Dátum 2026-11-08. Ellenőrzött főág
`d50d0d844379459903d1b1fcdd3c0e21473e1c0b`.
[Részletes angol jelentés és elfogadási lista](RELEASE_1_0_AUDIT.md).

A következő tervezett kiadás **1.0.0 stabil**. A jelenleg publikált verzió
0.2.0-alpha.1. Az 1.0.0 verzióra állítás a végleges jelölt előkészítéséhez tartozik.

## Reprodukált hibák

| Jel | Súly | Eredmény és szükséges javítás |
| --- | --- | --- |
| A1 | Magas | 48 kHz-es bemeneten a 11 917,593111 Hz-es szinuszból a teljes capture út 82,4079 Hz-et, azaz E2-t jelez. A 12 kHz-re ritkítás előtti egypólusú szűrő nem nyomja el eléggé a visszahajló összetevőket. Megfelelő szűrés és újramintavételezés kell, célzott regressziós tesztekkel. |
| A2 | Közepes | D3 után 65 ms-onként 16 hangminta érkezése mellett a régi kijelzés 520 ms után is él. A kevés új elemzési mintánál történő korai visszatérés kihagyja a 220 ms-os lejárat ellenőrzését. Minden feldolgozási ágon ellenőrizni kell a mért érték korát. |
| A3 | Közepes | A mentett numerikus beállítások NaN értéke átjut a toDouble és clamp kezelésen. Hibás szöveges A4-adatnál a program 440 helyett 430-at állít vissza. Konverzió-, végesség- és tartományellenőrzés, valamint sérült beállításokra indítási teszt kell. |
| A4 | Alacsony | A pontosan 700 Hz-es tiszta hang kiesik a megadott 25–700 Hz tartomány felső szélén. A becslés interpolációs túllövése miatt végponti szabály és teszt kell. |
| A5 | Közepes megerősítés | A csomagellenőrző összeveti a symlink célját a leltárral, de nem ellenőrzi, hogy a cél a csomagon belül marad-e. A macOS framework-linkek megtartása mellett ki kell zárni a kifelé mutató célokat és láncokat. Külső támadással kihasználható esetet ez az audit nem igazolt. |

A1 szintetikus magas hanggal, A2 szimuláltan lelassult adatérkezéssel,
A3 sérült vagy külsőleg módosított beállítással jelentkezik. A szokásos
gitárhasználatban való előfordulási gyakoriságuk külön vizsgálandó.

## Ellenőrzések

A teljes alkalmazás AppleClang 21 és Qt 6.11.2 mellett lefordult. A négy
CTest és a kilenc Python-teszt sikeres, a Python-források szintaxisa érvényes.
A dokumentáció helyi fájlhivatkozásai működnek. Öt mintavételi frekvencián
(12/22,05/44,1/48/96 kHz) a tiszta B0/E2/E4 jelek legnagyobb mért eltérése
körülbelül 0,45 cent volt. Ez 15 szintetikus eset eredménye.

Új windowsos futtatás, valódi hangszeres mérés, teljesítményprofilozás,
sanitizer/fuzz-vizsgálat és telepítőfrissítési próba nem történt ebben az
auditban. Az ideiglenes mérőkód kikerült a munkapéldányból. A PDF-eket az
előző dokumentációs munka vizsgálta, az 1.0.0 jelölthöz frissíteni kell őket.

## Az 1.0.0-ig szükséges munka

1. A1–A3 javítása regressziós tesztekkel, A4 végponti szabálya és A5 csomagellenőrzése.
2. Valódi gitár/basszus próbák minden hirdetett profilra, külön B0/E1, hét húros
   gitár, zaj, gyenge alaphang, húr- és oktávváltás. Előre rögzített pontossági
   és beállásiidő-kritériumok szükségesek.
3. Több PCM-formátum, 44,1/48/96 kHz, csatornák, darabolt keretek, eszközcsere,
   mikrofontiltás és visszaengedélyezés próbái mindkét rendszeren.
4. Késleltetés és GUI-terhelés mérése. A jelenlegi ablak 341 ms hangot tartalmaz,
   az elemzés a GUI szálán fut. Mért szükség esetén külön feldolgozószál kell.
5. Alpha → 1.0 frissítés, újratelepítés, beállításmegőrzés és eltávolítás
   tiszta rendszereken, a támogatási mátrix rögzítésével.
6. Sérült beállítások helyreállítása és döntés a rejtett, mentett A4-kalibrációról.
7. Egységes verzió-/kiadási csatorna, stable buildadatok, pontos tag és csomagazonosság.
8. Függőség-karbantartás áttekintése, végleges bináris-/forrás-/licencazonosság.
9. Angol és magyar kézikönyv, README, kiadási jegyzet és állapot frissítése.
10. A végleges taghez tartozó CI és telepítők ellenőrzése, stabil GitHub Release.

Az aláírás és notarizáció telepítési termékdöntés. A VST3, Intel Mac,
Windows ARM, Linux-telepítő és kézi célhúrválasztás külön mérföldkőként kezelhető.
A történeti alpha-naplókat és a már kiadott taget meg kell őrizni.

## Függőségek karbantartása

A mellékelt FFmpeg 7.1 után a 7.1.1 és 7.1.2 biztonsági javításokat kapott.
Az 1.0.0 előtt frissítési döntés és új bináris-/forrás-/licencellenőrzés kell.
A program QAudioSource PCM-bemenetet használ, médiafájl- vagy URL-dekódert
nem kínál. A felsorolt upstream hibák alkalmazáson keresztüli elérhetőségét
ez az audit nem igazolta. [FFmpeg biztonsági javítások](https://ffmpeg.org/security.html).
