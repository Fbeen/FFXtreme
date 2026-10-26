# Verslag FFXtreme: waar we staan

Laatst bijgewerkt: 2026-10-26, op Franks laptop. Het werk is gedaan op 2026-09-30.
Achtergrond, pinnen, timings en hoe je bouwt staan in [CLAUDE.md](../CLAUDE.md). Dit verslag
vertelt wat er gebeurd is en waar je verder gaat.

## Verder gaan op een andere computer

```
git clone https://github.com/Fbeen/FFXtreme      # of in een bestaande map: git fetch
cd FFXtreme
git checkout overdracht-desktop
```

Start daar Claude Code en begin bijvoorbeeld met:

> Lees docs/verslag.md en CLAUDE.md en installeer de toolchain, zodat ik de firmware kan bouwen.

## Wat er gedaan is

1. **De repo is opnieuw ingelezen.** De video-engine is ongewijzigd die van Keir Fraser (FF OSD v1.9, voor
   de Amiga). Upstream heeft daarna alleen nog GCC 12-fixes gedaan.
2. **De videotimings van de ST zijn onderzocht:** kleur 50 Hz (313 lijnen, ~15,67 kHz), kleur 60 Hz
   (263 lijnen, ~15,79 kHz) en mono (501 lijnen, ~35,8 kHz). Low en medium hebben dezelfde sync.
3. **De oorzaak van het verticale springen van de OSD is gevonden en opgelost:** de VSYNC- en HSYNC-interrupts
   haalden elkaar in. Nu leest de HSYNC-interrupt het VSYNC-niveau uit. (Het horizontale trillen
   speelt bij Frank niet.)
4. **Extra:** een eigen verticale offset voor 60 Hz ("60Hz V.Off"), een nette herstart bij een monitorwissel
   en de upstream GCC 12-fixes.
5. **Fixes aan het ST-toetsenbord:** ontvangst via een interrupt en een buffer, muis- en joystickbytes worden
   genegeerd, er blijven geen toetsen meer "hangen", en er is echt Ctrl **én** Alt nodig.
6. **De toolchain is geïnstalleerd** (MSYS2, arm-none-eabi-gcc 13.3). De firmware bouwt zonder fouten
   (~15,6 KB).
7. **De OSD toont bij elke floppyactiviteit info.** Dat lijkt op te lossen met FF.CFG, zie
   [osd-instellingen.md](osd-instellingen.md).
8. **Flashen bleek niet mogelijk via de micro-USB van de Blue Pill** (de STM32F103 heeft geen
   USB-bootloader). Er is een ST-Link V2 of een USB-serieel adapter (3,3V) nodig.

Alle codewijzigingen zitten in commit `d50363e` ("Fix vertical OSD jitter on Atari ST and improve
keyboard handling").

## Status

- **Gebouwd, maar nog niet op de hardware getest.** Er is nog niet geflasht.
- Frank zou de FF.CFG-instellingen uitproberen; de uitkomst is nog niet bekend.
- Aan het eind noemde Frank "5516". Dat was mogelijk een nummer van een adapter of chip. Dat is niet
  uitgezocht: vraag ernaar.

## Volgende stappen

1. **Flasher regelen.** Een ST-Link V2-kloon is het makkelijkst; een USB-serieel adapter (3,3V) kan ook.
2. **Eerst een back-up van de huidige flash maken.** De opbouw van de config is gewijzigd, dus de
   nieuwe firmware valt bij de eerste start terug op de fabrieksinstellingen. Lees de huidige
   H/V-offsets uit de back-up (config-pagina op 0x0800FC00).
3. **Flashen en testen:** kleur 50 Hz, kleur 60 Hz en mono. Is het verticale springen weg? Werken de
   Ctrl+Alt-sneltoetsen goed? Stel daarna V.Off, 60Hz V.Off en Mono V.Off opnieuw af.
4. **Als het werkt:** `st-sync-fix` / `overdracht-desktop` samenvoegen in `master`.
5. **Mogelijke vervolgstappen** (zie ook "Open punten" in CLAUDE.md):
   - De OSD alleen tonen als de image-naam verandert (een firmware-oplossing, als FF.CFG niet bevalt
     omdat het OLED dan ook uitgaat).
   - Ctrl+Alt+Delete reset nu de STM32, niet de ST. Is dat de bedoeling?
   - B8/B9 staan als open-drain LOW. Is dat de bedoeling?
