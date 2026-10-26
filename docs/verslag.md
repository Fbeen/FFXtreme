# Verslag FFXtreme: waar we staan

Laatst bijgewerkt: 2026-10-26, op de Linux-desktop. Achtergrond, pinnen, timings, bouwen en flashen
staan in [CLAUDE.md](../CLAUDE.md). Dit verslag vertelt wat er gebeurd is en waar je verder gaat.

## Verder gaan op een andere computer

```
git clone https://github.com/Fbeen/FFXtreme      # na de verhuizing: RetroLoft/FFXtreme
cd FFXtreme
git checkout overdracht-desktop                  # na de release: master
```

## Sessie 1 (2026-09-30, laptop): onderzoek en eerste fixes

- De oorzaak van het verticale springen van de OSD gevonden en opgelost: de VSYNC- en
  HSYNC-interrupts haalden elkaar in. Nu leest de HSYNC-interrupt het VSYNC-niveau uit.
- Eigen verticale offset voor 60 Hz, een nette herstart bij een monitorwissel, de GCC 12-fixes van upstream.
- Fixes aan het ST-toetsenbord: interrupt en buffer, muis- en joystickbytes negeren, geen
  "hangende" toetsen, echt Ctrl **én** Alt.
- Gebleken: flashen kan niet via de micro-USB van de Blue Pill.

## Sessie 2 (2026-10-26, desktop): flashen, testen, verder verbeteren

Alles hieronder is geflasht en door Frank op de hardware getest.

1. **Back-up** van de oude flash gemaakt (`/home/frank/retro/ffxtreme-flash-backup-2026-10-26.bin`)
   en de oude offsets eruit gelezen. Die zijn nu de fabrieksinstellingen.
2. **Het beeld staat stil**, in kleur en mono. Er is nog een heel klein horizontaal bibberen;
   acceptabel. De echte oplossing zou hardware zijn (zie CLAUDE.md).
3. **FF.CFG:** de OSD gaat na 5 seconden uit (`display-off-secs = 5`), en de lege regel boven de titel
   is weg (`osd-display-order = 0d,1`).
4. **OSD-menu:** sluit na 1 minuut zonder toets, of met Ctrl+Alt+Undo. Wijzigingen vervallen dan.
5. **Toetsenbord:** grotere buffer (256 bytes) en een schone herstart als er toch bytes verloren gaan.
6. **TOS, opstartdrive en geluid** worden onthouden, ook na uit/aan. EmuTOS (F4) werkt nu na een koude start.
7. **Oude bug gevonden:** bij elke opslag van de instellingen herstartte de STM32 (de flash-schrijfroutine
   stopte niet bij een oneven aantal bytes). Daardoor resette de ST soms onverwacht, bijvoorbeeld bij S.
8. **Mono/stereo-meldingen** waren omgedraaid en toonden soms de oude stand. B0 hoog = mono.
9. **Ctrl+Alt+Delete** reset nu de ST, niet meer de STM32.
10. **Pinnen:** TOS (A3/A4) en geluid (B0) worden actief aangestuurd. B8/B9 (ongebruikte
    Gotek-knoplijnen) worden niet meer laag getrokken.

## Volgende stappen: release v2.0

1. Documentatie bijwerken (README, RELEASE_NOTES, CLAUDE.md, dit verslag). **Gedaan.**
2. `FW_VER` naar 2.0, bouwen, flashen, controleren dat het menu "v2.0" toont.
3. Samenvoegen in `master`, oude branches opruimen.
4. Frank verhuist de repo naar de organisatie RetroLoft (GitHub: Settings → Transfer ownership).
   Daarna de remote en de links bijwerken.
5. Tag `v2.0` en een GitHub-release met `.hex` en `.bin`. De downloadpagina op de wiki ook bijwerken.
