# CLAUDE.md

Communiceer met Frank in het **Nederlands**. Frank is de oorspronkelijke auteur van dit project
(zijn eerste grote project, 2022) en pakt het in 2026 weer op om het te verbeteren.

## Project

FFXtreme is firmware voor een STM32F103C8T6 (Blue Pill, Cortex-M3, 72 MHz, bare metal C) die een
On Screen Display over het videobeeld van een **Atari ST** legt, met informatie van een Gotek met
FlashFloppy. Het is een licht omgebouwde versie van Keir Frasers
[FF OSD](https://github.com/keirf/flashfloppy-osd) (gemaakt voor de Amiga). De video-engine is
ongewijzigd die van Keir v1.9; upstream heeft daarna alleen nog GCC 12-fixes gedaan, die hier
inmiddels zijn overgenomen.

ST-specifieke uitbreidingen: bediening via het ST-toetsenbord (Ctrl+Alt+toets), OSD in kleur
(15 kHz) en mono (35,8 kHz), reset, opstarten van interne/externe drive, TOS-switch (F1–F4),
mono/stereo-geluid.

### Belangrijke bestanden
- `src/main.c`: sync- en video-engine (timers TIM1/TIM2/TIM4, DMA, SPI), hoofdlus, ST-hotkeys.
- `src/atari.c`: ST-toetsenbord meelezen (PB11, USART3, 7812,5 baud) met een interrupt en een ringbuffer.
- `src/i2c.c`: I2C-slave naar de Gotek: LCD-emulatie (0x27) of het eigen FF OSD-protocol (0x10).
  Op Franks print zijn A0-A1 vast verbonden → altijd het eigen protocol.
- `src/config.c`, `src/default_config.c`, `inc/config.h`: instellingen en het OSD-menu.
- `hardware/*.zip`: Eagle-ontwerpen + schema-PDF's (FlashFloppy-uitbreiding, TOS-switcher, YM2149).
- `docs/osd-instellingen.md`: naslag voor Frank: FF.CFG-opties, OSD-menu en sneltoetsen.
- `docs/verslag.md`: verslag van wat er gedaan is en waar je verder gaat. **Lees dit eerst.**

### Pinnen (zie ook het schema in `hardware/FlashFloppy expansion.zip`)
A8 = HSYNC (heet in de code "csync"), B14 = VSYNC, B15 = video kleur (SPI2, 9 MHz),
A7 = video mono (SPI1, 36 MHz), B5 = mono detect (LOW = mono), B11 = ST-toetsenbord,
B6/B7 = I2C naar Gotek, A9/A10 = seriële console/bootloader, A3/A4 = TOS (ROML/ROMH, push-pull),
C13 = boot drive (push-pull), B0 = geluid (push-pull, HOOG = mono), B3 = reset-lijn naar de ST
(open-drain, werkt; het label "B1/RESET" in het schema is niet in gebruik).
B8/B9 = BTN_LEFT/RIGHT op JP5: wel doorverbonden met de Gotek, maar ongebruikt (knoppen gaan via
I2C) → ingang met pull-up. Jumpers: A0–A1 = eigen I2C-protocol, A1–A2 = config naar fabrieksinstellingen.
De Eagle-`.sch`-bestanden zijn XML en goed met Python uit te lezen (nets per pin). Let op: de echte
printen wijken op punten af van de schema's (bijv. de 10k-pulls op TOS/geluid/reset).

## Atari ST-videotimings (uit eigen kennis, niet online geverifieerd)
| Modus | Lijnfrequentie | Lijnen/beeld | Beeldfrequentie |
|---|---|---|---|
| Kleur 50 Hz (low/medium) | ~15,67 kHz (512 cycli @ 8 MHz) | 313 | ~50,05 Hz |
| Kleur 60 Hz (low/medium) | ~15,79 kHz (508 cycli) | 263 | ~60 Hz |
| Mono (high) | ~35,8 kHz (224 cycli) | 501 | ~71,4 Hz |

Low en medium hebben identieke sync; alleen de pixelklok verschilt (8 en 16 MHz; mono 32 MHz).
HSYNC en VSYNC zijn aparte signalen, allebei actief laag. De bovenrand is bij 60 Hz ~29 lijnen korter dan bij 50 Hz.

## Bouwen (Linux, Franks desktop)
`arm-none-eabi-gcc` en `make` staan erop; gewoon `make` in de repo-root.

## Bouwen (Windows, Franks laptop)
Toolchain: MSYS2 UCRT64 `arm-none-eabi-gcc` (13.3) + newlib + make, via pacman:
```
pacman -S mingw-w64-ucrt-x86_64-arm-none-eabi-gcc mingw-w64-ucrt-x86_64-arm-none-eabi-newlib make
```
Bouwen **in de MSYS2-shell**. `make` vanuit Git Bash faalt met "Cannot create temporary file in C:\WINDOWS\":
```
MSYSTEM=UCRT64 /c/msys64/usr/bin/bash -lc 'cd <repo-pad> && make'
```
Het resultaat komt in `src/FFXtreme.hex/.bin/.elf`. De build gebruikt `-Werror`. Op Franks laptop is geen Python geïnstalleerd.

## Flashen
- **Niet** via de micro-USB van de Blue Pill: de ROM-bootloader van de F103 kent geen USB, en Windows
  toont dan "Onbekend USB-apparaat (descriptor mislukt)". Dat is onderzocht op 2026-09-30.
- Frank gebruikt een **USB-serieel adapter (CH340, 5V)** op `/dev/ttyUSB0`: TX→A10, RX→A9, GND,
  5V→5V-pin (alleen als de ST uit staat). A9/A10 zijn 5V-tolerant. Frank zit in de groep `dialout`,
  dus geen `sudo` nodig:
  ```
  stm32flash -b 57600 -w src/FFXtreme.hex -v /dev/ttyUSB0
  ```
  Frank zet de BOOT0-jumper op 1 + reset vóór het flashen, en daarna terug op 0 + reset.
  "Failed to init device" = BOOT0/reset nog niet gedaan; "Failed to open port" = adapter niet ingeplugd.
- Een ST-Link V2 (SWD) kan ook.
- Back-up van de oorspronkelijke v1.9-flash (64 KB, tweemaal gelezen, identiek):
  `/home/frank/retro/ffxtreme-flash-backup-2026-10-26.bin`. Terugzetten:
  `stm32flash -b 57600 -w <backup.bin> -v -S 0x08000000 /dev/ttyUSB0`.
- Werkwijze die Frank wil: **één wijziging tegelijk** bouwen → flashen → laten testen → committen.

## Stand van zaken (2026-10-26)
Alles hieronder is **op Franks hardware getest en werkt** (branch `overdracht-desktop`):
1. **Verticaal springen van de OSD opgelost** (Franks hoofdklacht). De VSYNC- en HSYNC-IRQ haalden
   elkaar in; nu leest de HSYNC-IRQ in ST-modus het VSYNC-niveau uit (`st_hsync()`), de VSYNC-EXTI is
   in ST-modus gemaskeerd. Het Amiga-pad (`amiga_csync()`) is ongewijzigd. Beeld staat stil in kleur en mono.
2. `frame_lines` telt de lijnen per beeld → bij 60 Hz wordt `v_off60` gebruikt (menu "60Hz V.Off").
3. Wisselen mono/kleur: mono detect-pin 100 ms anders → `system_reset()`.
4. ST-toetsenbord: RX via IRQ + ringbuffer van 256 bytes; bij overloop of USART-overrun wordt de buffer
   geleegd en Ctrl/Alt vergeten; muis-/joystickpakketten (0xF6–0xFF + data) overgeslagen; losgelaten
   toetsen altijd doorgegeven; Ctrl **én** Alt vereist.
5. Fabrieksinstellingen = Franks offsets (H.Off 82, V.Off 241, 60Hz V.Off 212).
6. OSD-menu sluit na 1 minuut zonder toets, of met Ctrl+Alt+Undo (wijzigingen vervallen, `config_cancel()`).
   Esc is bewust **niet** gebruikt: dat ververst op de GEM-desktop de diskinhoud.
7. TOS (F1–F4), boot (B) en geluid (S) worden direct in de flash opgeslagen (`config_save_atari()`),
   alleen bij een echte wijziging. Bij een open menu worden de onopgeslagen menuwijzigingen niet mee opgeslagen.
8. Bug uit 2022 opgelost: `fpec_write()` liep eindeloos door bij een oneven grootte (struct config is
   375 bytes) → watchdog-reset van de STM32 na elke opslag.
9. Watchdog (min. 213 ms) wordt tijdens de resetpuls van 250 ms bijgevoed (`st_reset_wait()`).
10. Ctrl+Alt+Delete reset de ST via de resetlijn, niet meer de STM32.
11. A3/A4/B0 push-pull (eerst zwakke pulls), pas na het laden van de config ingesteld; B8/B9 ingang.
12. Mono/stereo-meldingen klopten niet (B0 hoog = mono); de pin wordt niet meer teruggelezen.
13. GCC 12+-fixes van upstream.

FF.CFG op Franks USB-stick: `display-off-secs = 5`, `osd-display-order = 0d,1` (geen lege
submap-regel boven de titel). Kopieën: `FF.CFG.bak`, `FF.CFG.bak2`.

Het horizontale "bibberen" is klein en voor Frank acceptabel. Oorzaak: de SPI-bitklok loopt niet in fase
met de ST. De structurele oplossing is hardware: de Blue Pill op de 8 MHz van de ST laten lopen
(HSE-bypass, PLL ×9 = 72 MHz; 512 ST-klokken = 576 STM32-klokken = precies 72 SPI-bits per lijn).
Frank wil daar nu geen moeite in steken.

## Bezig: release v2.0
Plan: (1) documentatie bijwerken, (2) `FW_VER` naar 2.0, bouwen, flashen, menu toont "v2.0",
(3) samenvoegen in `master` en `st-sync-fix`/`overdracht-desktop` opruimen, (4) Frank verhuist de repo
zelf via GitHub "Transfer ownership" naar de organisatie **RetroLoft** (oude links worden doorgestuurd;
nooit meer een nieuwe `Fbeen/FFXtreme` aanmaken), daarna remote en links in README/docs bijwerken,
(5) tag `v2.0` + GitHub-release met `.hex` en `.bin`. Eerdere release: `v1.9` (2022-12-11).
De downloadlink in de README wijst naar de wiki (aparte repo): die pagina moet ook bijgewerkt worden.
Pushen, taggen en releases alleen als Frank het zegt.

## Mogelijke vervolgstappen
- De OSD alleen tonen als de image-naam verandert (firmware-oplossing, als FF.CFG niet bevalt).
- Hardware: klok van de ST naar de Blue Pill tegen het horizontale bibberen (zie hierboven).

## Codestijl
Linux/Keir-stijl: 4 spaties inspringen, geen tabs (atari.c had tabs; die zijn opgeschoond),
C99 met `-Wdeclaration-after-statement` (declaraties bovenaan een blok), commentaar in het Engels.
