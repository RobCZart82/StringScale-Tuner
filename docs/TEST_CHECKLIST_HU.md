# Kiadás előtti tesztlap – Robert

Ez kitöltendő tesztlap: az üres vagy „nem tesztelt” sor nem sikeres eredmény.
A fájlt lemásolhatod és szövegszerkesztővel kitöltheted, majd visszaküldheted.

## A kipróbált csomag

- Csomag fájlneve:
- Verzió és Build a Névjegyből:
- Pontos revision a build-info.json fájlból:
- Windows/macOS verzió és processzor:
- Hangkártya/mikrofon, bemenet és csatorna:
- Gitár/basszus, hangolási profil:
- Teszt dátuma:

Eredményként írd be: OK / HIBA / NEM TESZTELT. Hibánál néhány mondat,
képernyőkép és az ismétlés lépései segítenek.

## Indítás és beállítások

| Próba | Eredmény | Megjegyzés |
| --- | --- | --- |
| Kicsomagolás, indítás Qt/CMake/Visual Studio nélkül | NEM TESZTELT | |
| Mikrofonengedély megadása; tiltás után érthető üzenet | NEM TESZTELT | |
| Magyar → English → Magyar, nyitott ablakokban is | NEM TESZTELT | |
| Világos/sötét téma, olvasható feliratok | NEM TESZTELT | |
| Újraindítás után nyelv, téma, eszköz és csatorna megmarad | NEM TESZTELT | |
| Kis ablak / kijelzőnagyítás: nincs levágott lényeges felirat | NEM TESZTELT | |
| Névjegy: helyes verzió, logó és szignó, külön polgári név nélkül | NEM TESZTELT | |

## Standard hathúros gitár

Egyenként pengesd meg a húrokat. Jegyezd fel az oktávos nevet, a skála
mozgását, a centmutató irányát, és hogy stabil hangnál mennyire nyugszik meg.
Ha van másik bevált hangolód, hasonlítsd össze a jelzéseket.

| Húr | Elvárt név | Eredmény | Mutató, pontosság, reakcióidő |
| --- | --- | --- | --- |
| 6 | E2 | NEM TESZTELT | |
| 5 | A2 | NEM TESZTELT | |
| 4 | D3 | NEM TESZTELT | |
| 3 | G3 | NEM TESZTELT | |
| 2 | B3 | NEM TESZTELT | |
| 1 | E4 | NEM TESZTELT | |

## Jel és eszközkezelés

| Próba | Eredmény | Megjegyzés |
| --- | --- | --- |
| Kis le-/felhangolásnál helyes irányt jelez | NEM TESZTELT | |
| Csendben nincs hamis „Jól szól!” vagy középre állt aktív mutató | NEM TESZTELT | |
| Gyenge és túl erős bemenet: érthető jelszintjelzés | NEM TESZTELT | |
| Erős felharmonikusú pengetés: nincs indokolatlan oktávugrálás | NEM TESZTELT | |
| E3 felvétellel vagy megfelelő hangforrással E3-ként jelenik meg | NEM TESZTELT | |
| Gitár az 1., majd 2. bemeneten: helyes csatornaválasztás | NEM TESZTELT | |
| Másik csatorna hangja nem befolyásolja a kiválasztott bemenetet | NEM TESZTELT | |
| Hangeszközváltás, USB kihúzás és visszacsatlakoztatás | NEM TESZTELT | |
| Pár perces folyamatos használat: nincs fagyás vagy erős akadozás | NEM TESZTELT | |

## Hibajegy mintája

- Melyik csomag/build:
- Mit csináltam:
- Mit vártam:
- Mi történt:
- Minden alkalommal ismételhető-e:
- Kép / felvétel / további megjegyzés:
