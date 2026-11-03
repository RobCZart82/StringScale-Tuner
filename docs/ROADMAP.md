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
- [ ] M2: Add user-selectable input channel for multichannel DI interfaces.
- [ ] M2: Add optional manual target-string selection and clearer clip hold.
- [ ] M3: Polish multi-resolution retina layouts, accessibility and localization.
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
