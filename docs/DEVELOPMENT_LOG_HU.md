# Fejlesztési és javítási napló

## 2026-11-05 – Windows licenc-hash ellenőrzés sorvégjavítása

A #22 Windows CI-je a licencszöveg SHA256-tesztjén hibázott. A naplóban
szereplő hash pontosan megegyezett az upstream LF szöveg CRLF-re alakított
változatának hash-ével. A .gitattributes a licenses szövegekre explicit LF
sorvéget ír elő, így a Windows core.autocrlf nem változtatja meg a forrásadatot.
Az ellenőrzőösszeg-teszt szigorú marad, nem normalizáljuk vagy gyengítjük.

## 2026-11-05 – Licencmellékletek és tényleges runtime-audit előkészítése

A csomagok licenses/ mappája teljes LGPL v3, GPL v3 kísérő és LGPL v2.1
szövegeket kapott, az upstream forrás URL-jével és SHA256 azonosítóval. A
felhasználói telepítő a jogi mellékleteket megtartja. Az upstream FFmpeg-tag
a licencszöveg eredetét jelöli, nem állítja a telepített bináris pontos verzióját.

Új runtime-licence-audit.json készül a ténylegesen stage-be került binárisok
útvonalaival, hash-eivel és előzetes kategóriáival. Az ismeretlen komponens
unclassified marad; a release_cleared mindig false, a hiányzó auditlépések
külön szerepelnek. A fájl csak a developer ZIP-ben van, nem települ az app mellé.

A teljes Qt harmadik fél közleménylista, pontos FFmpeg-konfiguráció és
megfelelő forrás, illetve Windows runtime-terjesztési feltételek ellenőrzése
továbbra is nyitott kiadási feltétel. Ez részfeladat, nem lezárt jogi audit.
Hét Python-teszt ellenőrzi a csomagszűrést, binárisleltárt és a licencszövegek
eredetellenőrző összegeit.

## 2026-11-04 – Külső audit ellenőrzése és elavult leírások javítása

A StringScale-Tuner_audit_report.txt jelentést a 7554198 főági állapottal
vetettük össze. A verzió-/állapoteltérés igazolt, több auditpont ennek ismétlése.
A hardveres jegyzőkönyv állítólagos UTF-8 sérülése nem reprodukálható: szigorú
dekódolással érvényes, helyettesítő karakter nincs benne. A jelentésben nem
szerepelt forráscommit; az eredeti pillanatkép ezért nem rekonstruálható biztosan.

PROJECT_STATUS_HU.txt, BUILD_HU.md, ARCHITECTURE.md, README, roadmap és kiadási
jegyzet javítva. A verziót a CMake határozza meg; a műszerek sorrendje, zöld
jelölő, ±50 centen túli befogott mutató, csatorna-/nyelv-/méretválasztó, CI és
telepítők leírása a kódot követi. A történeti tesztnapló külön jelölést kapott;
a régi események valódi verzióhivatkozásait nem írjuk át utólag.

A mért hang és a legközelebbi célhúr különbsége pontosítva: az abszolút oktáv
kijelzése segít az eltérés felismerésében, de nem bizonyítja a fizikai húr
azonosságát. Nincs motor- vagy GUI-viselkedésváltoztatás. Részletes auditértékelés:
AUDIT_REVIEW_HU.md.

## 2026-11-04 – Letisztított telepített felhasználói dokumentáció

Robert jóváhagyta, hogy a publikált felhasználói csomagban csak a HU/EN PDF,
rövid README és szükséges licencanyag szerepeljen. A telepítő ezért a hitelesen
ellenőrzött fejlesztői stage-ből külön, szűrt másolatot készít: a futásidejű
komponensek változatlanok, a teszt- és hardveres dokumentumok, SDK-SBOM, valamint
build-/leltár-JSON-ok kimaradnak. A manuals fejlesztői README-je sem települ.

A CI külső leltárból vizsgálja a telepített fájlokat, és a dokumentációs
könyvtár pontos tartalmát is ellenőrzi. A leltár külön audit artifact, a régi
ellenőrzött ZIP külön developer artifact. Öt célzott Python-teszt ellenőrzi a
Windows/Mac futásidő megtartását, jogi anyagokat, szűrést és a hibás/hiányos
bemenetek elutasítását. Ismeretlen jövőbeli komponens nem tűnhet el csendben.
A korábban telepített tesztlapokat nem töröljük automatikusan.

## 2026-11-04 – Hűvös világos téma és fix felületméretek

Robert kérésére a világos fő háttér enyhén kékes törtfehér (#F2F6FB), a panelek
fehérek (#FFFFFF), a finom szegélyek #CBD5E1 árnyalatúak. A sötét paletta és a
hangoló motor nem változott.

A Beállításokban 75%, 100%, 125%, 150% választható; a korábbi 1000×850 logikai
pixeles méret a 125%-os alap. A főablak méretei rendre 600×510, 800×680,
1000×850 és 1200×1020. Az ablakok fixek; betűk, ikonok, vezérlők, térközök és
festegetett műszerek együtt skálázódnak. A Névjegy/Hang/Beállítások ablakokra
is érvényes, az eredeti font- és layoutértékekből, felhalmozódó kerekítési hiba nélkül.
A méret azonnal mentődik; az operációs rendszer DPI-beállításától független.

A kijelző munkaterületére túl nagy fokozatok letiltottak. Indításkor és
kijelzőváltáskor a program a legnagyobb elférő, legfeljebb kért fokozatot használja.
A legkisebb felülethez 600×510 logikai pixel és a rendszer ablakkeretének helye kell.

A GUI-teszt mind a négy méretet mindkét nyelvvel és témával ellenőrzi: pontos
fix ablakméret, kilógó vezérlők, levágott QLabel-feliratok és minden hangszerprofil
nevének elférése, valamint külön folyamatból visszaolvasott mentett méret.
A teljes helyi Apple Silicon build és CTest 4/4 sikeres. A legkisebb főablak,
Beállítások és Névjegy, valamint a 125%/150% előnézetek vizuálisan ellenőrizve.

## 2026-11-04 – Zöld listakeret és közös natív alkalmazásikon

A lenyíló listák egérrel kijelölt vagy billentyűzettel aktív sora 1 logikai pixeles,
a téma aktív zöldjét követő keretet kap, tömör zöld kitöltés nélkül. A listaelem
mérete nem változik hoverkor. Az összes QComboBox és a fogaskerék menüje egységes
keretes jelölést használ. Az egérmozgatás nem változtatja meg a mentett profilt;
a GUI-teszt valódi mouse-move eseménnyel és Escape-pel is ellenőrzi ezt, valamint
HU/EN, világos/sötét popup-előnézeteket ment.

Új közös ikon: a sötét felület egyszerűsített miniatűrje, felső jelszintmérővel,
analóg mutatóval és alsó húrskálával. Eredeti SVG-masterből 16–1024 pixeles
PNG-k, Windows ICO és macOS ICNS készültek, átlátszó, lekerekített sarkokkal.
A legkisebb méreteknél a mutató és ív erősebb, az apró osztások elmaradnak.
A Windows EXE és telepítő, a Qt app, valamint a Mac bundle az új ikont használja.
A telepítő-smoke teszt a natív ikon jelenlétét is vizsgálja.

A helyi teljes Apple Silicon build és CTest 4/4 sikeres; a popupok és az ikon
vizuálisan ellenőrizve. A hangoló motor változatlan.

## 2026-11-04 – Windows EXE és macOS PKG próbatelepitők

A GitHub Actions a már ellenőrzött ZIP-stagingből natív telepítőt is készít.
Windows: Inno Setup 6, felhasználónkénti telepítés, HU/EN varázsló, Start menü,
opcionális asztali ikon és eltávolító. macOS: pkgbuild/productbuild, Applications
app és külön dokumentációs mappa; nem relocatable bundle. A telepítők nem kapnak
kiadói aláírást vagy notarizációt. Nincs automatikus GitHub Release-publikálás.

A két színes, nyolcoldalas PDF-kézikönyv bekerült a repositoryba és a csomagokba.
A ZIP-ellenőrző megköveteli jelenlétüket. A telepítő mellé SHA256 és forrásazonosítós
JSON manifest kerül. A staging tartalmát újraellenőrizzük a ZIP leltárához képest.
A Mac PKG kibontott payloadját is fájlonként és aláírás szerint ellenőrizzük.

A Windows CI telepít, ellenőrzi és indítja az appot, majd eltávolít; a Mac CI
a saját eldobható VM-jén telepít, ellenőrzi és mikrofon nélkül indít. A helyi
csomagépítés nem telepít semmit Robert gépére. Részletek: INSTALLERS_HU.md.
A telepítők Qt-alapú minimuma Windows 10 1809 és macOS 12.0; a Mac deployment
target explicit 12.0, így egy új helyi SDK nem emeli észrevétlenül a minimumot.

## 2026-11-04 – Főág összevezetése és első kiadás előkészítése

A #11/#12/#13 egymásra épülő PR-ok fordított sorrendű összevonása után a main
csak a #11 állapotát tartalmazta. Az összevezető ág megőrzi a #12/#13 történetét
és visszahozza a már elfogadott villogásjavítást, Mac-aláírást, antracit témát
és sík listajelölőket a mainhez készülő PR-ba. Nincs új hangolási algoritmus.

Robert sikeres hardveres visszajelzései külön tesztjegyzőkönyvbe kerültek,
a konfigurációk és bizonyítékok hatókörének megadásával. A kiadási jegyzet
elkészült tervezetként; a checklist a ténylegesen igazolt sorokat jelöli.
A csomagoló a tesztjegyzőkönyvet és kiadási jegyzetet is mellékeli.

A kész ZIP upload előtt külön ellenőrzést kap: SHA256 és fájlnév, manifest,
platform, tiszta forrásazonosító, összes leltározott fájl tartalma és mérete,
symlinkek, kötelező futtatható fájlok és dokumentáció. A licencmellékletek
teljes auditja és a végleges kiadás publikálása továbbra is hátralévő feladat.

## 2026-11-04 – Sík, keret nélküli lenyílólista-jelölők

Robert kérésére a lenyíló listák jobb szélén a natív, kidomborodó gomb helyett
átlátszó hátterű, keret nélküli SVG-chevron látható. Világos témán sötét,
sötét témán törtfehér vonalat használ. A 32 logikai pixeles kattintható terület
megmarad, a szövegnek külön jobb oldali helyet tartunk fenn. A főablak és a
Beállítások/Hang ablak összes QComboBox mezőjére egységesen érvényes.

## 2026-11-04 – Jóváhagyott kékes antracit sötét téma

Robert jóváhagyása alapján a fő háttér #252D38, a panelek és a párbeszédablakok
háttere #303B49. A Beállítások, Hang és Névjegy tartalmát 1 logikai pixeles
#718096 keret választja el a háttérben maradó főablaktól. Az operációs rendszer
ablakmozgatása, címsora és átméretezése megmarad. A fő szöveg #EDF1F5, a
másodlagos állapotszöveg #B8C4D2, a türkiz kiemelés #39DCC1. A világos téma
megjelenése nem változik; a váltás a nyitott párbeszédablakokra is érvényes.
A helyi GUI-előnézeteken a keret és a világos SVG-k vizuálisan ellenőrizve;
a teljes helyi build és CTest 4/4 sikeres.

## 2026-11-04 – Mac-próbacsomag aláírásának javítása

A teljes Qt-deploy után a helyi bundle `codesign --verify --deep --strict`
ellenőrzése hibát adott: „code has no resources but signature indicates they must
be present”. A korábbi sérült alkalmazás figyelmeztetésnek ez is lehetséges oka;
a karanténjelölés eltávolítása önmagában nem javítja a hibás aláírást.

A közös csomagoló most az összes függőség telepítése után ad-hoc aláírja a teljes
Mac-bundle-t, majd szigorú ellenőrzést futtat. Hiba esetén a csomagolás leáll.
Csak ezután készül a fájlleltár és a ZIP ellenőrzőösszege. A build-info külön
jelöli az ad-hoc aláírást, a kiadói aláírás és notarizáció továbbra is hamis.
Ez nem helyettesíti a nyilvános kiadás előtti Developer ID/notarizációs döntést.

## 2026-11-04 – Villogó élő mérés: csomagszünet és csend szétválasztása

Robert SSL 2+ interfészes videója alapján a mért hang és a mutató időnként eltűnik,
majd visszatér. A kódban az üres 65 ms-os frissítés eddig azonnal csendet jelzett;
az interfész rövid csomagszünete így hamis kijelzőtörlést okozhatott.

- Új csomag hiányában a jelszint megmarad; 250 ms valódi adatkimaradáskor a
  teljes elemzési előzmény és kijelzés törlődik. Az újraindulás friss ablakot vár.
- Egy bizonytalan becslés vagy rövid halkulás legfeljebb 220 ms-ig tartja meg az
  utolsó érvényes hangot. Sikertelen mérések nem hosszabbítják ezt a határidőt.
- A minimum jelszint kapuja 3 dB hiszterézist kapott. Digitális túlvezérléskor,
  eszközleállításkor és csatornaváltáskor nincs hangtartás.
- A timer a sorban álló PCM-adatokat is kiolvassa; nem feltételezi a readyRead
  és a felületfrissítés azonos ütemezését. A UI továbbra sem mutat Hz-értéket.
- Új eszközfüggetlen regressziós teszt a valódi PCM-dekódolási és DSP-útvonalon:
  100 ms-os csomagok / 65 ms-os frissítés, tartós és timer elől rejtett adatkimaradás,
  másik hanggal visszatérés, zajos becslés, küszöbkörnyéki jel, csend, clipping, reset.
- Helyi Apple Silicon Release build és CTest 4/4 sikeres, beleértve az új
  hangbemeneti regressziós és a meglévő GUI-előnézeti tesztet.
- A videó önmagában nem bizonyítja minden kihagyás okát. A javított csomagot
  ugyanazzal az SSL 2+ bemenettel ismét tesztelni kell; hardveres javulás még nincs igazolva.

## 2026-11-04 – Azonosítható alpha tesztcsomag és kiadási ellenőrzőlista

- Közös CMake-verzióforrás: 0.2.0-alpha.1; a Névjegy ezt és rövid buildazonosítót mutat.
  A macOS belső numerikus bundle-verziója 0.2.0. Helyi módosított forrás dirty jelölést kap.
- Verziózott platform/build ZIP, mellette SHA256; a csomagban build-info.json,
  tényleges fájlleltár, indulási útmutató és kitölthető, kezdetben NEM TESZTELT tesztlap.
- A Qt SDK beszállítói SBOM-jai auditsegédletként bekerülnek, ha elérhetők.
  Ez nem helyettesíti a végleges licenc- és forráselérhetőségi ellenőrzést.
- Új RELEASE_CHECKLIST_HU.md rögzíti az első publikálás előfeltételeit.
  A workflow továbbra is tesztartifactokat készít; nem publikál Release-t.

## 2026-11-04 – Polgári név szöveges kiírásának eltávolítása a Névjegyből

Robert kérésére a Névjegyben a polgári nevet kizárólag az eredeti SVG-szignó
képviseli. A külön szöveges sorban csak a RobCZart82 név marad.
A szignó akadálymentes leírása sem írja ki a polgári nevet: „A szerző szignója” /
„Author signature”. Az eredeti SVG-források és a téma szerinti változatok megmaradnak.
Ez felülírja a korábbi szöveges szerzőnévre vonatkozó felületi követelményt.

## 2026-11-04 – Választható bemeneti csatorna USB/DI hangkártyákhoz

- A Hang / Audio ablakban választható a gitárhoz használt csatorna, magyar és angol
  felirattal. A lista a ténylegesen megnyitott formátum csatornáit mutatja; egyetlen
  csatornánál nem váltható, aktív bemenet nélkül le van tiltva.
- A program a támogatott többcsatornás formátumot kéri a korábbi mono kérés helyett.
  Elsőként a maximális hirdetett csatornaszámot próbálja 48k Float, majd Int16
  formátumban; ha ezek nem támogatottak, az eszköz preferált formátumára vált vissza.
- Csak a kiválasztott csatorna kerül a hangolóba és a jelszintmérőbe. A másik,
  üres csatorna nem halkítja, az ellenfázisú csatorna nem oltja ki a gitár jelét.
  A túlvezérlésjelzés is a választott csatornára vonatkozik.
- A választás azonnal, eszközazonosítónként mentődik. Alapérték az 1. csatorna;
  ha a mentett csatorna az új formátumban nem létezik, szintén az 1. lesz aktív.
- Élő csatornaváltáskor a hangforrás nem indul újra, a korábbi hangmagasság- és
  jelszintelőtörténet törlődik. A félbemaradt PCM-keret megmarad, hogy a csatornák
  bájthatárai ne tolódjanak el.
- Szintetikus tesztek: üres társcsatorna, ellenfázisú E2, másik csatorna clippingje,
  hibás csatornaszám, üres keret és nem véges minták. A GUI-teszt a kiválasztott
  csatorna nyelvváltás alatti megőrzését is ellenőrzi.
- Helyi Apple Silicon build és CTest 3/3 sikeres; a magyar/világos és angol/sötét
  Hang ablak képei vizuálisan ellenőrizve. Valódi USB/DI hardveres teszt még szükséges.

## 2026-11-04 – Világos szignó sötét témához

Az eredeti Alairas-Vektor.svg változatlanul megmarad. Új párja az
Alairas-Vektor-light.svg: csak a kitöltőszín változott #211A19-ről #eee9dc-re,
a világos GYR-logóhoz igazodva. A görbék, a méretek, a viewBox és az átlátszó
háttér azonos. A Névjegy témaváltáskor automatikusan a megfelelő változatot
rajzolja; a korábbi világos szignópanel kikerült a sötét témából.
A világos SVG az alkalmazásba csomagolt erőforrás, érvényességét a meglévő
GUI-teszt is ellenőrzi. Ez felülírja a korábbi naplóbejegyzés világos paneles megoldását.

## 2026-11-04 – Kétnyelvű felület, fogaskerék-menü és Névjegy

- Magyar / English választó a Beállítások / Settings ablakban, a témaválasztó mellett.
  Az összes saját felirat, műszerszöveg, hangolási profilnév, menü, hibaüzenet és
  akadálymentes elnevezés követi a nyelvet; a hangnevek és húrszámok nem változnak.
- Nyelvváltáskor a már nyitott Beállítások, Hang és Névjegy ablak is azonnal frissül.
  A bemenet és a hangolási profil nem vált át és nem indul újra.
- A választás mentődik. Első indításkor magyar rendszernyelvnél magyar,
  minden más rendszernyelvnél angol az alapérték; ezt a felhasználó felülírhatja.
- A fogaskerék menüje: Beállítások / Settings, Hang / Audio, Névjegy / About.
  A nyelv és téma azonnal mentődik; az audio-küszöbökhöz külön Mentés / Mégse tartozik.
- A Névjegyben verzió, szöveges szerzőnév, eredeti GYR SVG-logó és eredeti vektoros
  szignó. Világos és sötét logó témához igazítva; a sötét szignó világos panelen.
  Az SVG-k beágyazott erőforrások, nincs külső fájl- vagy hálózatfüggés.
- Helyi Apple Silicon teljes build és CTest 2/2 sikeres. A GUI-teszt nyitott
  ablakok nyelvváltását, változatlan profil/bemenet választást, külön folyamatból
  visszaolvasott nyelvmentést és SVG-erőforrásokat ellenőriz; mindkét nyelv és téma
  PNG-előnézeteit menti. A teszt elkülönített, ideiglenes beállításprofilt használ.
- A CI a főág push eseményére és PR-módosításokra fut: ugyanazt a PR-commitot
  nem fordítja kétszer push és pull_request eseményből. A kötelező tesztnevek változatlanok.
- Valódi gitárjeles, kézi hangeszköz- és akadálymentességi ellenőrzés még szükséges.

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

## 2026-11-03 – Magyar és angol felület felvétele a tervekbe

Robert jóváhagyta a magyar–angol nyelvváltás tervét. A fogaskerékből megnyitott
beállításablakban a témaválasztóhoz hasonló legördülő kerül: „Nyelv / Language”,
„Magyar” és „English” lehetőségekkel. A váltás azonnal, újraindítás nélkül
frissítse a főablakot és a nyitott beállításablakot, a választás pedig mentődjön.
A nyelv független a világos/sötét témától; az oktávos hangnevek, húrszámok és
a mérés nem változnak, Hz-érték egyik nyelven sem jelenhet meg.
Az ütemterv részletes követelményeket és ellenőrzőlistát tartalmaz mindkét
nyelvhez és témához: [ROADMAP.md](ROADMAP.md).
Állapot: fejlesztési terv, a nyelvváltás ebben a dokumentációs változtatásban
még nem valósult meg.

## 2026-11-03 – VST3 bővítés felvétele a tervekbe

Robert elfogadta a standalone elsődlegessége mellett a közös motorra épülő,
másodlagos VST3-változat tervét. Első plugin cél: Windows x64 és macOS
Universal (Intel + Apple Silicon); később Windows ARM és kísérleti Linux x64.
A stabil standalone és az elfogadott új GUI megelőzi a plugin kiadását.
A közös motor valós idejű átalakítása, a plugin bemenete, továbbengedés/némítás,
GUI és hostkompatibilitási tesztek részletesen az [ütemtervben](ROADMAP_HU.md#standalone-first-és-vst3-bővítés--elfogadott-irány-2026-11-03) szerepelnek.
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
