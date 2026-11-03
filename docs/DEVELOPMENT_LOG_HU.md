# Fejlesztési és javítási napló

## 2026-11-03 – Arculati SVG-k és a Névjegy / About terve

Robert átadta a GYR-logó sötét és világos változatát, valamint a polgári
nevéhez tartozó vektoros szignót. Az eredeti SVG-k a resources/branding
mappába kerültek. XML-szerkezetük ellenőrizve: nincs bennük beágyazott
raszterkép, script vagy külső hivatkozás.

A tervezett jobb felső fogaskerék-menüben a Beállítások / Settings és
Hang / Audio alatt lesz a Névjegy / About. Az ablakban mindkét arculati elem
felhasználható, az eredeti képarány megőrzésével és témához illő kontraszttal.
A feliratok a tervezett magyar–angol nyelvválasztást követik.
Részletek: [ütemterv](ROADMAP.md) és [arculati fájlok](../resources/branding/README.md).

Állapot: forrásfájlok archiválva és követelmények rögzítve; a menü és a
Névjegy ablak megvalósítása még hátravan.

## 2026-11-03 – VST3 bővítés felvétele a tervekbe

Robert elfogadta a standalone elsődlegessége mellett a közös motorra épülő,
másodlagos VST3-változat tervét. Első plugin cél: Windows x64 és macOS
Universal (Intel + Apple Silicon); később Windows ARM és kísérleti Linux x64.
A stabil standalone és az elfogadott új GUI megelőzi a plugin kiadását.
A közös motor valós idejű átalakítása, a plugin bemenete, továbbengedés/némítás,
GUI és hostkompatibilitási tesztek részletesen az [ütemtervben](ROADMAP.md#standalone-first-és-vst3-bővítés--elfogadott-irány-2026-11-03) szerepelnek.
Ez dokumentált fejlesztési irány; plugin még nem készült. Keretrendszerváltást
nem határoztunk el, a kiválasztott SDK és függőségek licence külön ellenőrizendő.

## 2026-11-03 – Elfogadott GUI-koncepció

Állapot: Robert által elfogadott grafikai és működési irány, megvalósításra vár. Ez a bejegyzés követelményeket rögzít; nem jelenti a funkciók elkészültét.

### Cél és vizuális irány

A StringScale Tuner legyen egyszerű, gyerekek számára is jól olvasható, önálló Windows- és macOS-hangoló. Nagy, kontrasztos feliratok, kevés kezelőelem és világos visszajelzések segítsék a használatot. A felület alapja Robert saját, három egymás fölé rendezett műszert ábrázoló vázlata és az ebből készült, elfogadott világos látványterv (StringScale-GUI-koncepcio-v3.png).

A látványterv elrendezési referencia. A skálák geometriáját, a mérést és a jelölők helyét a program számolja; a renderelt kép nem mérési specifikáció.

### Elrendezés

1. Fejléc: alkalmazásnév és a Beállításokat megnyitó fogaskerék.
2. Választók: hangszer/hangolás és hangbemeneti eszköz.
3. Fent: vízszintes bemeneti jelszintmérő.
4. Középen: nagy, analóg, félköríves ±50 centes mutatós hangoló.
5. Alul: egyenes, vízszintes frekvenciaskála a kiválasztott hangszer húrjelöléseivel.

### Felső jelszintmérő

- A bemenő hang erősségét folyamatosan követő, jól olvasható kijelzés.
- MIN és MAX jelölés mutassa a megbízható hangoláshoz használható jeltartomány alsó és felső határát.
- A megfelelő tartomány legyen vizuálisan kiemelve; szöveg vagy ikon is segítse az értelmezést.
- Példa megfelelő állapotra: „Megfelelő jelszint”.
- A megjelenített hangerő a bemeneti jelre vonatkozik.
- A végleges küszöbök még nincsenek meghatározva. A korábbi renderben szereplő számok nem elfogadott határértékek; ezeket valódi hangbemenettel kell ellenőrizni.

### Középső analóg hangoló

- A központi műszer kapja a legnagyobb területet.
- Tartomány: −50…+50 cent; középen 0. Balra túl mély, jobbra túl magas hang.
- Jól látható mutató, erős főosztások és a nulla körüli kiemelt tartomány.
- A mért hang saját oktávos neve nagy betűkkel jelenjen meg, például A2. Ez a mért hang neve, nem pusztán a legközelebbi célhúr felirata.
- A hozzá tartozó húr száma külön is megjelenhet, például „5. húr”.
- Egyszerű visszajelzések: „Túl mély”, „Jól szól!”, „Túl magas”. Ne kizárólag szín közvetítse az állapotot.
- A pontos hangoltsági tolerancia és a mutató simítása megvalósításkor ellenőrizendő; a látványterv színes sávjának szélessége nem rögzített algoritmikus küszöb.

### Alsó húrskála

- Egyetlen egyenes vonalon jelenjen meg a pillanatnyi mérés és a kiválasztott hangszer célhúrjainak helye.
- A jelölések felett a húrcsomagolásokon használt húrszám, alattuk az oktávos hangnév szerepeljen.
- Standard hathúros gitárnál balról jobbra:

| Húr száma | Oktávos hangnév |
| --- | --- |
| 6 | E2 |
| 5 | A2 |
| 4 | D3 |
| 3 | G3 |
| 2 | B3 |
| 1 | E4 |

- A hangszer/hangolás legördülő választása frissítse a húrjelöléseket.
- A mérés jelölője folyamatosan kövesse a frekvenciaváltozást; a célhúrok jelölései maradjanak rögzítettek az adott hangolásban.
- A húrskála az abszolút hangmagasságot, az analóg mutató a célhanghoz viszonyított finom eltérést mutatja.
- A skála a meglévő logaritmikus frekvencialeképezést kövesse; a rajzolt célpozíciókat ne a látványterv képpontjai határozzák meg.

### Frekvenciaszámok elhagyása – kötelező döntés

A felhasználói felületen sehol ne jelenjen meg numerikus frekvencia vagy Hz egység. Ez vonatkozik a főablakra, az alsó skálára, a mérési feliratokra, az eszköztippekre és a beállításokra is. A skála címe lehet „Húrok”.

A mért oktávos hangnév, húrszám és a centes skála megmarad. A frekvencia belső számítása továbbra is szükséges a hangmagasság-érzékeléshez és a jelölő mozgatásához.

### Világos és sötét téma

A fogaskerékkel megnyitott ablakban a Beállítások → Megjelenés résznél két választható téma legyen:

- Világos: a jóváhagyott koncepció törtfehér háttere, sötét, kontrasztos feliratokkal.
- Sötét: grafitszürke háttér, világos, kontrasztos feliratokkal.

Mindkét témában ugyanaz az elrendezés, nagy betűméret és jól látható jelölésrendszer maradjon. A váltás azonnal érvényesüljön, a választás pedig a következő indításra is mentődjön. A pontos színkódok még nincsenek rögzítve.

### Oktávhelyesség

Az E2 és E4 külön céljelölés, az oktávszám nem hagyható el. Ha a mérés E3, a mért hang kijelzőjén E3 szerepeljen; ne fedje el ezt egy automatikusan kiválasztott célhúr neve. A felharmonikusok miatti téves alaphangbecslést külön is vizsgálni kell. A grafika önmagában nem oldja meg az érzékelő oktávhibáit.

### Megvalósítási és ellenőrzési lista

- [ ] A főablak átrendezése a három egymás fölötti műszerre.
- [ ] Nagy, kontrasztos, gyerekek számára is olvasható feliratok mindkét témában.
- [ ] Minden frekvenciaszám és Hz-felirat eltávolítása a felhasználói felületről.
- [ ] Hangszer/hangolás választásakor a megfelelő húrszámok és oktávnevek megjelenítése.
- [ ] A mért oktávos hangnév elkülönítése a célhúr nevétől.
- [ ] MIN–MAX bemeneti tartomány és megfelelő jelszint visszajelzés.
- [ ] Világos/sötét témaváltás a fogaskerék ablakában, azonnali alkalmazással és tartós mentéssel.
- [ ] Mind a hat standard gitárhúr ellenőrzése; a B3 hozzáadása a frekvenciatesztekhez.
- [ ] E2/E3/E4 és erős felharmonikusú jelek ellenőrzése.
- [ ] Valódi gitárral a skálamozgás, mutató, csend, gyenge jel és túlvezérlés ellenőrzése.
- [ ] Letölthető Windows x64 és macOS Apple Silicon próbacsomag, a teljes Qt alkalmazással és függőségeivel.

### Kiinduló technikai ellenőrzés

A 2026-11-03-án ellenőrzött main commit: `c22e4a17aeb50b0cfa9a14fa0623f75f2dde8b10`. A v0.1.0 ZIP mind a 23 fájlja azonos a repóval. Az akkori Actions csak a motor tesztjeit fordítja és futtatja (`STRINGS_BUILD_GUI=OFF`), teljes grafikus buildet és letölthető csomagot nem készít. A motor tesztjei Windows, macOS és Linux alatt sikeresek voltak; a helyi CMake/CTest ellenőrzés szintén sikeres (1/1). Ez nem igazolja a GUI és a valódi hangbemenet működését.

Kapcsolódó dokumentumok: [architektúra](ARCHITECTURE.md), [ütemterv](ROADMAP.md), [buildleírás](BUILD_HU.md), [korábbi teszteredmények](TEST_RESULTS.md).


## 2026-11-03 – Teljes asztali build és próbacsomag előkészítése

- A főág védelme aktív: PR, három kötelező motoros teszt, lezárt review-beszélgetések, törlés és force push tiltása; nincs bypass.
- Új Desktop preview workflow: Windows x64 és macOS arm64 teljes Qt build, motoros tesztek, függőségek telepítése és letölthető ZIP artifactok.
- CMake telepítéskor a Qt deployment script gyűjti a futásidejű függőségeket.
- A szintetikus hangteszt B3 és E3 esetekkel bővült, felharmonikusokkal is.
- Magyar próbaverzió-letöltési és kézi tesztleírás: PREVIEW_HU.md.
- Az új grafikai felület, a Hz-feliratok eltávolítása és a témaváltás továbbra is megvalósítandó. Ez a lépés a jelenlegi GUI fordíthatóságát és csomagolását ellenőrzi.


## 2026-11-03 – Az elfogadott GUI első megvalósítása

- Felső MIN–MAX jelszintmérő, nagy középső félköríves ±50 centes hangoló,
  alsó logaritmikus húrskála húrszámokkal és oktávnevekkel.
- Hz-értékek és a kalibrációs frekvenciavezérlő eltávolítása a felületről.
  A korábban mentett kalibráció belső használata megmarad.
- Külön mért hangnév és célhúr; E3 mérés E3 névvel jelenik meg.
- Világos és sötét megjelenés a fogaskerékből, azonnali alkalmazással és
  QSettings mentéssel. A téma a Mégse gomb ellenére is mentődik, ezt a dialog jelzi.
- Csendben nincs középre állított, hamisan hangoltságot jelző mutató.
  50 centen túli eltérésnél a mutató a szélen marad és szöveg jelzi a túllépést.
- GUI renderellenőrzés mikrofonnyitás nélkül: világos/sötét, hangolt A2,
  E3 oktávpélda és csend. A CI PNG-ket ment külön artifactba.
- A valódi gitárjeles és kézi akadálymentességi ellenőrzés továbbra is szükséges.

- Ellenőrzés: Windows x64 teljes CI build, CTest 2/2 és próbacsomag sikeres;
  a generált világos/sötét képek vizuálisan ellenőrizve, a levágott státuszszöveg javítva.
- Helyi macOS Apple Silicon: Qt 6.8.3 teljes fordítás, CTest 2/2 és függőségcsomagolás sikeres.
  Az új Apple SDK-ból hiányzó régi AGL linkfüggőség elhagyva a Qt importált
  OpenGL céljáról. A GitHub macOS futtatógépének elérhetősége ettől független.
