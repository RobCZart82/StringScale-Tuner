# Első kiadás előtti ellenőrzőlista

Tervezett első próbaverzió: 0.2.0-alpha.1, Windows x64 és macOS Apple Silicon.
Az alpha elnevezés nem igazolja a hardveres tesztek sikerét. Ez a lista nem
publikál automatikusan GitHub Release-t.

- [ ] A végső commit minden kötelező tesztje és mindkét asztali buildje sikeres.
- [ ] A csomag verziója, Névjegye és build-info.json adatai egyeznek.
- [ ] A SHA256 ellenőrzőösszeg a letöltött belső ZIP-re egyezik.
- [ ] Robert Windows laptopján és Mac minijén kitöltött TEST-RESULTS-HU.md.
- [ ] Mind a hat gitárhúr, az oktávok, gyenge/erős jel és csatornaválasztás próbája.
- [ ] Eszközváltás, mikrofonengedély és kihúzás/visszacsatlakoztatás próbája.
- [ ] A próbák során talált kiadást akadályozó hibák javítva és újraellenőrizve.
- [ ] A ténylegesen csomagolt függőségek licenc- és forráselérhetőségi ellenőrzése
      kész; az előzetes THIRD_PARTY_NOTICES.md helyett teljes kiadási melléklet.
- [ ] A támogatott rendszerek és minimumverziók ellenőrizve, dokumentálva.
- [ ] Aláírás/notarizáció állapota és az indítás menete egyértelműen dokumentálva.
- [ ] Rögzített forráscommit, verziócímke, kiadási jegyzet és ismert korlátozások.
- [ ] A jóváhagyott csomagok a GitHub Release-hez csatolva.

Az aláírt telepítő, további architektúrák, Linux és VST3 külön mérföldkő.
Egy első tesztelői alpha lehet kicsomagolható archívum; a stabil, széles körű
kiadás indítási és telepítési elvárásai ennél szigorúbbak.
