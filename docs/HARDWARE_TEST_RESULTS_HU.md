# Hardveres visszajelzések – 2026-11-04

Forrás: Robert saját, ebben a fejlesztési beszélgetésben közölt próbái.
Ezek felhasználói teszteredmények, nem laboratóriumi pontosságmérés.

## Konfigurációk

| Rendszer | Bemenet | Azonosított próba |
| --- | --- | --- |
| Mac mini M1 / macOS | SSL 2+ | Villogásjavítást tartalmazó helyi alpha; a pontos macOS-verzió nincs megadva |
| Windows 10 laptop / x64 csomag | Behringer UCG-102 Guitar Link | 0.2.0-alpha.1, GitHub build 41 |

A Mac próba forrása a helyi csomag manifestje szerint f16a0f5;
a Windows build41 a vele azonos alkalmazáskódot tartalmazó PR merge-forrásból
készült. Az új antracit téma és sík chevron későbbi módosítás: ezek képeit és
HU/EN témaváltását automatikusan ellenőrizzük, az új grafika hardveres elfogadása
nincs külön visszaigazolva.

## Eredmények

| Próba | Eredmény | Bizonyíték / hatókör |
| --- | --- | --- |
| Korábbi villogás az SSL 2+ bemeneten | OK | Robert szerint a javított változat jól működik |
| E2, A2, D3, G3, B3, E4 standard húrok | OK mindkét konfiguráción | Mind a hat megfelelő hangmagasságban felismerve |
| Külső hangolókkal összevetés | OK | Robert külön hangolókészüléket és csíptetős hangolót is figyelt; kijelzésük egyezett az appéval |
| Mind a hat húr ±25, ±30, ±40 cent eltéréssel | OK mindkét konfiguráción | Robert kifejezetten mindkét interfészt és rendszert megerősítette |
| D3 húr lehangolva C3-ra | OK | C3 mért hang, 4. húr / D3 cél, túl mély és több mint 50 cent; a próba rendszere külön nincs megadva |
| Hathúros Drop D | OK | Robert visszajelzése; platformonkénti bontás nincs megadva |
| Szándékos oktáveltérés, interfész kihúzás/visszadugás, csatornaváltás és beállítások megőrzése | OK felhasználói összefoglaló szerint | Robert a hátralévő működési próbák teljesítését megerősítette; részletes mérési jegyzőkönyv nincs |
| Nyelvváltás és világos/sötét téma | OK macOS és Windows | Robert mindkét platformra külön megerősítette |

## Nem igazolt általánosítások

Basszus, hét húros gitár, a többi alternatív hangolás, más interfészek,
operációsrendszer-minimumok és hosszú távú terhelés nincsenek ezzel igazolva.
A mikrofonengedély kifejezett megtagadásának kezelése, szélsőséges jelszintek,
minden felharmonikusviszony és kijelzőnagyítás sem kapott külön visszaigazolást.
Az automatizált DSP-tesztek ezeket a hardveres próbákat nem helyettesítik.
