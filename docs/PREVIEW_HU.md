# Asztali próbaverzió

Az Actions **Desktop preview** munkafolyamata a teljes Qt alkalmazást fordítja
Windows x64 és macOS Apple Silicon (arm64) célra, majd a Qt futásidejű
függőségeivel együtt csomagolja. A meglévő Core DSP tests változatlanul fut.

## Letöltés és indítás

1. A GitHub Actions alatt nyiss meg egy sikeres Desktop preview futást.
2. Az Artifacts részből töltsd le a gépedhez tartozó csomagot
   (GitHub-bejelentkezés szükséges lehet).
3. Bontsd ki az artifactot, majd a benne található verziózott ZIP-et is.
   Az alpha csomag neve verziót, platformot és buildszámot tartalmaz.
4. Windows: a csomag bin mappájában indítsd a StringScaleTuner.exe fájlt.
   A mellékelt DLL-eket és almappákat hagyd mellette.
5. macOS: a csomag StringScaleTuner.app alkalmazását másold az Alkalmazások közé.
   A rendszer kérésére engedélyezd a mikrofont.
6. A csomag TEST-RESULTS-HU.md tesztlapját töltsd ki. A Névjegy verziója és
   Build sora, illetve a build-info.json pontos forrásazonosítója azonosítja a próbát.
7. A .zip.sha256 a belső, verziózott ZIP ellenőrzőösszege, nem a GitHub által
   köré csomagolt artifact ZIP-é. Windows: Get-FileHash <fájl.zip> -Algorithm SHA256;
   macOS: shasum -a 256 <fájl.zip>.

Helyi Qt-, CMake- vagy Visual Studio-telepítés nem szükséges. A csomagok
kísérleti, fejlesztői próbaverziók, nem telepítők. Nincs kiadói aláírás vagy
Apple-notarizáció; az operációs rendszer figyelmeztethet. macOS-en a
Rendszerbeállítások → Adatvédelem és biztonság alatt engedélyezhető a
megbízható forrásból letöltött alkalmazás megnyitása.

## Mit ellenőriz a CI?

- Teljes GUI fordítása, motor tesztjei, futásidejű függőségek telepítése.
- macOS arm64 architektúra és mikrofonhasználati leírás jelenléte.
- Windows fő futtatható fájl és alapvető Qt DLL/plugin fájlok jelenléte.

A CI most mikrofonnyitás nélküli GUI-indítást is végez, és világos/sötét PNG
előnézeteket készít (GUI-* artifactok). A felület az elfogadott három műszeres
elrendezést és témaváltást használja, Hz-értékek nélkül. Ez még nem valódi
gitárjeles teszt; a témaváltást, mentést és a bemenetet kézzel is ellenőrizni kell.

## Kézi próba

- Válassz bemenetet és standard hathúros gitárt.
- Ellenőrizd külön az E2, A2, D3, G3, B3 és E4 húrt.
- Figyeld a skálajelölő folytonos mozgását és a centmutató irányát.
- Ellenőrizd a két E húr és az E3 mérés elkülönítését.
- Próbálj csendet, gyenge jelet, erős felharmonikusú pengetést és túl erős jelet.
- Hibajelzéshez add meg az operációs rendszert, bemeneti eszközt,
  az Actions-futás linkjét és a tapasztalt viselkedést.

A Qt külön licencelt keretrendszer. Nyilvános kiadás előtt a harmadik fél
licenceinek és forráselérhetőségének teljes kiadási ellenőrzése még szükséges.


## Nyelv és Névjegy (2026-11-04)

A fogaskerék Beállítások / Settings pontjában választható a Magyar / English
nyelv és a világos/sötét téma. Mindkettő azonnal alkalmazódik és mentődik.
A Hang / Audio külön kezeli a bemeneti jelszintküszöböket. A Névjegy / About
ablak a szerző eredeti vektoros logóját és szignóját mutatja.

Próbáld ki a két nyelvet mindkét témában, a nyitott ablakokkal is; újraindításkor
ellenőrizd a nyelv és téma visszaállítását. A GUI-* artifactok már a Névjegy,
a Hang és a Beállítások képeit is tartalmazzák.


## Többcsatornás hangkártya kipróbálása

A fogaskerék → Hang / Audio ablak Bemeneti csatorna / Input channel mezőjében
válaszd ki a gitár csatlakozójához tartozó csatornát. A csatornaválasztás azonnal
mentődik, a jelszintküszöbök továbbra is a Mentés / Save gombbal. A főablakban
választott eszközhöz külön megjegyzi a csatornát. Első használatkor az 1. csatorna aktív.

A csatornák számozása az operációs rendszer/Qt által adott sorrendet követi;
a program nem ismeri a hangkártya fizikai csatlakozóinak feliratait. A lista
csak a ténylegesen megnyitott formátumban elérhető csatornákat tartalmazza.

Próba: gitár az első, majd a második bemeneten; váltáskor a megfelelő
jelszint és hang jelenjen meg. A másik bemenet hangja vagy túlvezérlése ne
befolyásolja a kiválasztott gitárcsatornát. Ellenőrizd az eszközváltást,
kihúzást/visszacsatlakoztatást és az újraindítás utáni csatornaválasztást is.
