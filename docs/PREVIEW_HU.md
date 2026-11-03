# Asztali próbaverzió

Az Actions **Desktop preview** munkafolyamata a teljes Qt alkalmazást fordítja
Windows x64 és macOS Apple Silicon (arm64) célra, majd a Qt futásidejű
függőségeivel együtt csomagolja. A meglévő Core DSP tests változatlanul fut.

## Letöltés és indítás

1. A GitHub Actions alatt nyiss meg egy sikeres Desktop preview futást.
2. Az Artifacts részből töltsd le a gépedhez tartozó csomagot
   (GitHub-bejelentkezés szükséges lehet).
3. Bontsd ki az artifactot, majd a benne található preview ZIP-et is.
4. Windows: a bin mappában indítsd a StringScaleTuner.exe fájlt.
   A mellékelt DLL-eket és almappákat hagyd mellette.
5. macOS: a stage mappában található StringScaleTuner.app alkalmazást
   másold az Alkalmazások közé. A rendszer kérésére engedélyezd a mikrofont.

Helyi Qt-, CMake- vagy Visual Studio-telepítés nem szükséges. A csomagok
kísérleti, fejlesztői próbaverziók, nem telepítők. Nincs kiadói aláírás vagy
Apple-notarizáció; az operációs rendszer figyelmeztethet. macOS-en a
Rendszerbeállítások → Adatvédelem és biztonság alatt engedélyezhető a
megbízható forrásból letöltött alkalmazás megnyitása.

## Mit ellenőriz a CI?

- Teljes GUI fordítása, motor tesztjei, futásidejű függőségek telepítése.
- macOS arm64 architektúra és mikrofonhasználati leírás jelenléte.
- Windows fő futtatható fájl és alapvető Qt DLL/plugin fájlok jelenléte.

Ez nem automatizált GUI-indítás vagy valódi gitárjeles teszt. Az elfogadott
új GUI és témaváltás még külön fejlesztési lépés; ez a csomag a jelenlegi
prototípust teszi kipróbálhatóvá.

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
