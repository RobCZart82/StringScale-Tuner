# Roadmap

- [x] M0: Original C++20 pitch-detector, tuning reference model, CI-core tests.
- [x] M0: Implement Qt vector widgets for analog gauge, horizontal pitch map,
      on-line red pitch dot, and input level meter (source code only).
- [x] M0: Audio capture integration draft and basic preset/settings workflow.
- [ ] M1: Compile and launch Qt GUI on Windows 10/11 and macOS (Intel/ARM).
- [ ] M1: Test USB DI interfaces; inspect sample formats, device switching,
      real microphone permission and dynamic unplug behavior.
- [ ] M2: Ground-truth pitch accuracy with guitar and bass at E2/E1/B0,
      detunings, transients, vibrato, noise and weak fundamentals.
- [ ] M2: Improve latency: DSP thread, adaptive analysis window, resampler.
- [x] M2: Add user-selectable input channel for multichannel DI interfaces
      (implementation and synthetic tests; hardware validation still pending).
- [ ] M2: Add optional manual target-string selection and clearer clip hold.
- [ ] M3: Polish multi-resolution retina layouts and accessibility.
- [ ] M3: Add instant, persistent Magyar / English language selection in Settings
      alongside the light/dark theme selector; see the detailed requirements below.
- [ ] M3: GUI integration tests and platform build jobs.
- [ ] M4: Signed/notarized macOS .app/.pkg and Windows installer, open-source
      license notices and reproducible release workflow.


## Standalone-first és VST3 bővítés – elfogadott irány (2026-11-03)

A fő termék továbbra is az ingyenes, önálló StringScale Tuner. A VST3 másodlagos
kiadási forma, ugyanarra a közös C++ hangoló motorra építve. Ez fejlesztési terv,
nem elkészült plugin vagy igazolt platformkompatibilitás.

### Megvalósítási sorrend

1. Stabil standalone próbaverzió Windows x64 és macOS Apple Silicon alatt,
   valódi gitárjeles tesztekkel.
2. Az elfogadott, gyerekek számára is jól olvasható GUI, Hz-értékek nélkül,
   világos/sötét témával és mentett megjelenési beállítással.
3. A közös motor felkészítése a valós idejű pluginfeldolgozásra.
4. VST3 prototípus, majd Windows x64 (Intel/AMD) és macOS Universal
   (Intel x86_64 + Apple Silicon arm64) kiadás.
5. Windows ARM bővítés külön hosttesztekkel; Linux x64 későbbi,
   kezdetben kísérleti standalone/plugin cél.

A standalone macOS Intel támogatása is cél; az Apple Silicon próbacsomag
önmagában nem igazolja ezt. Windows ARM alatt a natív ARM64 és az esetleges
ARM64EC igényt a választott hostok és SDK alapján külön kell eldönteni.
Linux ARM egyelőre nincs vállalt kiadási célként rögzítve.

### Közös felépítés és működés

- Közös, Qt-tól és a VST3 SDK-tól független hangmagasság-érzékelés,
  hangolási modell és mérési állapot.
- Standalone adapter: saját hangeszköz- és bemenetválasztás, mikrofonengedély.
- VST3 adapter: a hangot a fogadó zenei program adja; nincs saját
  mikrofonnyitás vagy hangeszközválasztás.
- A plugin alaphelyzetben változtatás nélkül továbbengedi a hangot.
  Külön „Némítás hangolás közben” kapcsoló tervezett.
- Mindkét változatban azonos vizuális irány: felső jelszintmérő, nagy analóg
  ±50 centes mutató, alsó húrszám/oktávnév skála, Hz-feliratok nélkül,
  világos és sötét téma.
- A Qt felület pluginablakba illesztését prototípussal kell ellenőrizni.
  Keretrendszerváltásról nincs döntés; JUCE-ra átállás nincs előírva.

### Műszaki feladatok és elfogadási feltételek

- [ ] Előre lefoglalt pufferek, a feldolgozó hívásban memóriafoglalás és
      blokkoló műveletek nélküli hangút; szükség esetén külön elemző szál.
- [ ] A hangszál és a GUI közötti biztonságos állapotátadás; a GUI ne
      késleltesse a host hangfeldolgozását.
- [ ] Eltérő mintavételek, változó blokkméret, mono/sztereó bemenet,
      mintavételváltás és feldolgozás újraindításának tesztjei.
- [ ] A továbbengedett hang változatlanságának és a némítás működésének tesztje.
- [ ] Több pluginpéldány egymástól független állapota, projektmentés és
      visszatöltés; a hangolás és némítás állapotának kezelése.
- [ ] VST3 validator és kijelölt fogadóprogramok alatti betöltési,
      ablakkezelési és hangfeldolgozási tesztek.
- [ ] Külön architektúra-ellenőrzés, csomagolás és letöltési útmutató
      minden támogatott célhoz.

A platform SDK-támogatása nem helyettesíti a konkrét fogadóprogrammal végzett
tesztet. A host és plugin architektúrájának kompatibilitását minden célon
ellenőrizni kell. macOS-en nem minden zenei program fogad VST3-at; az AU
formátum nincs ebben a mérföldkőben vállalva.

### SDK és licencelési irány

A közvetlen Steinberg VST3 SDK használata az elsőként vizsgálandó út.
MIT-licencű SDK-verziót kell kiválasztani és rögzíteni; a 3.8-tól elérhető MIT
licenc nem jelenti az összes kapcsolódó komponens azonos licencét. A Qt,
VSTGUI és más függőségek saját feltételeit, licenceit és kiadási mellékleteit
külön ellenőrizni kell a csomagok terjesztése előtt.

Források:
- [VST3 SDK és platformok](https://steinbergmedia.github.io/vst3_dev_portal/pages/What%2Bis%2Bthe%2BVST%2B3%2BSDK/Index.html)
- [VST3 licencelés](https://steinbergmedia.github.io/vst3_dev_portal/pages/VST%2B3%2BLicensing/Index.html)


## Névjegy / About ablak – elfogadott arculati irány (2026-11-03)

Állapot: tervezett funkció; az SVG-források a resources/branding mappában
rendelkezésre állnak, az ablak megvalósítása külön feladat.

- A jobb felső fogaskerék lenyíló menüt nyisson. A Beállítások / Settings és
  Hang / Audio elemek alatt legyen elérhető a Névjegy / About.
  Ez a jelenlegi, közvetlenül beállításablakot nyitó működés tervezett bővítése.
- A Névjegy használja Robert GYR-logóját és a polgári nevéhez tartozó szignót.
  A megadott logó SVG-címe: „GYR — Gyuricza Róbert”.
- Az eredeti vektorgrafikák maradjanak meg SVG-ként; a megjelenítés méretarányosan,
  az eredeti viewBox figyelembevételével történjen. Ne legyen nyújtás vagy
  előzetesen kis felbontású raszterképpé alakítás.
- Világos témában GYR-black.svg, sötét témában GYR-light.svg használható.
- Világos témában az eredeti Alairas-Vektor.svg sötét szignó, sötét témában
  az Alairas-Vektor-light.svg világos szignó jelenjen meg átlátszó háttérrel.
  Külön fehér vagy világos panel nem szükséges; az eredeti forrás megmarad.
- A logó és a szignó külön grafikai elem; a Névjegy elrendezésében mindkettő
  kapjon arányos méretet és elegendő helyet.
- A Névjegy szöveges feliratai kövessék a tervezett Magyar / English
  nyelvválasztást; a személynév és a grafikai elemek nem fordítandók.

### Megvalósítás és ellenőrzés

- [ ] Fogaskerék-menü és Névjegy / About menüpont.
- [ ] SVG-k csomagolása az alkalmazás erőforrásai közé, a felhasználó gépén
      külső SVG-fájlok vagy hálózati letöltés nélkül.
- [ ] Méretarányos, éles megjelenítés kis és nagy ablakban, nagy DPI mellett is.
- [ ] Kontraszt és vágás ellenőrzése mindkét témában.
- [ ] A logó és a szignó mellett szöveges, akadálymentesen olvasható szerzőnév.
- [ ] Magyar és angol Névjegy-feliratok ellenőrzése.

Forrásfájlok és használati megjegyzések: [branding README](../resources/branding/README.md).

## Magyar–angol nyelvváltás – elfogadott terv (2026-11-03)

Állapot: jóváhagyott követelmény, megvalósításra vár. A standalone GUI következő
fejlesztési lépései közé tartozik; a későbbi VST3 felületre is érvényes.

- A fogaskerékből megnyitható Beállítások / Settings ablakban a megjelenés
  választója mellett, azzal azonos jellegű legördülő legyen a nyelvválasztó.
- A mező felirata „Nyelv / Language”, a két lehetőség mindig „Magyar” és
  „English”, a pillanatnyi felületnyelvtől függetlenül.
- A váltás azonnal frissítse a főablak és a már nyitott beállításablak
  feliratait; ne igényeljen újraindítást.
- A választás mentődjön, és a következő indításkor is érvényesüljön.
- A nyelv és a világos/sötét téma egymástól független beállítás.
- A hangnevek oktávszámmal (E2, A2, D3 stb.), a húrszámok és a mért értékek
  jelentése nem változik. Hz-érték egyik nyelven sem jelenhet meg.
- A fordítás terjedjen ki a hangszer/hangolás választók saját felirataira,
  állapotokra, hangolási útmutatásokra, beállításokra, saját hibaüzenetekre,
  eszköztippekre és akadálymentes elnevezésekre. A rendszer által adott
  eszközneveket meg kell őrizni.
- A fordítás ne változtassa meg a mérés menetét, az aktív hangbemenetet
  vagy a kiválasztott hangolást. Az olvashatóság mindkét nyelven maradjon
  nagy betűs, kontrasztos és egyszerű.

### Ellenőrzési feltételek

- [ ] Magyar → English → Magyar váltás újraindítás nélkül, a nyitott
      beállításablakban is.
- [ ] A mentett nyelv visszaállítása az alkalmazás újraindításakor.
- [ ] Mindkét nyelv ellenőrzése világos és sötét témában.
- [ ] A hosszabb feliratok, az ékezetek és a figyelmeztetések nem vágódnak le.
- [ ] A hangnevek, oktávok, húrszámok és a mérési viselkedés változatlansága.
- [ ] Hiányzó fordítások és kevert nyelvű saját feliratok ellenőrzése.

2026-11-04-i megvalósítás: mentett választás hiányában magyar rendszernyelvhez
magyar, más rendszernyelvhez angol induló felület tartozik. A felhasználó ezt
bármikor felülírhatja. A nyelvváltás és a Névjegy első megvalósításának ellenőrzéseit
a [fejlesztési napló](DEVELOPMENT_LOG_HU.md) rögzíti.
