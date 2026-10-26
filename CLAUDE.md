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
B6/B7 = I2C naar Gotek, A9/A10 = seriële console/bootloader, A3/A4 = TOS, C13 = boot drive,
B0 = geluid, B3 = reset-lijn naar de ST.

## Atari ST-videotimings (uit eigen kennis, niet online geverifieerd)
| Modus | Lijnfrequentie | Lijnen/beeld | Beeldfrequentie |
|---|---|---|---|
| Kleur 50 Hz (low/medium) | ~15,67 kHz (512 cycli @ 8 MHz) | 313 | ~50,05 Hz |
| Kleur 60 Hz (low/medium) | ~15,79 kHz (508 cycli) | 263 | ~60 Hz |
| Mono (high) | ~35,8 kHz (224 cycli) | 501 | ~71,4 Hz |

Low en medium hebben identieke sync; alleen de pixelklok verschilt (8 en 16 MHz; mono 32 MHz).
HSYNC en VSYNC zijn aparte signalen, allebei actief laag. De bovenrand is bij 60 Hz ~29 lijnen korter dan bij 50 Hz.

## Bouwen (Windows)
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
- Wel met een **ST-Link V2** (SWD-pinnen, geen jumper nodig) of een **USB-serieel adapter 3,3V**
  (TX→A10, RX→A9, GND; BOOT0-jumper op 1 + reset, en na afloop terug op 0). De flash-targets in de
  Makefile (stm32flash, /dev/ttyUSB0) gaan uit van Linux.
- **Maak vóór de eerste flash van de nieuwe firmware een back-up van de flash** (met name de
  config-pagina op 0x0800FC00). De opbouw van de config is gewijzigd (veld `v_off60`), waardoor de CRC
  niet meer klopt en de config bij de eerste start teruggaat naar de fabrieksinstellingen. Lees
  Franks huidige H/V-offsets uit de back-up en geef ze aan hem door.

## Stand van zaken (branches `st-sync-fix` en `overdracht-desktop`)
Klaar, bouwt, maar **nog niet op hardware getest**:
1. **Verticaal springen van de OSD (Franks hoofdklacht) opgelost.** De oorzaak was dat de VSYNC-IRQ
   en de HSYNC-IRQ elkaar inhaalden als hun flanken bijna samenvielen, waardoor de lijntelling per beeld
   ±1 verschilde. Nu leest de HSYNC-IRQ in ST-modus het VSYNC-niveau uit (`st_hsync()`); de
   VSYNC-EXTI is in ST-modus gemaskeerd. Het Amiga-pad (`amiga_csync()`) is ongewijzigd.
2. Het aantal lijnen per beeld wordt geteld (`frame_lines`) → bij 60 Hz wordt de nieuwe instelling
   `v_off60` gebruikt (menu-item "60Hz V.Off", standaard 237).
3. Wisselen tussen mono en kleur: als de mono detect-pin 100 ms anders staat, volgt een
   `system_reset()`. Voorheen bleef de DMA naar de verkeerde SPI wijzen.
4. ST-toetsenbord: RX via een interrupt en een ringbuffer (er gingen bytes verloren); muis- en
   joystickpakketten (0xF6–0xFF + data) worden overgeslagen (konden als hotkeys gelezen worden,
   bijv. F1 = TOS-wissel + reset); losgelaten toetsen worden altijd doorgegeven (voorheen bleven
   toetsen "hangen" als je Ctrl/Alt eerst losliet); de controle eist nu Ctrl **én** Alt (voorheen was Alt alleen genoeg).
5. GCC 12+-fixes van upstream (`&_sdat[0]`-vergelijking, `-fno-tree-loop-distribute-patterns` voor util.o).

Het horizontale trillen is bij Frank geen probleem. Mocht het ooit spelen: de oorzaak is dat de vrijlopende
SPI-baudgenerator niet in fase loopt met de ST. Het structurele idee is de STM32 laten meelopen op
de 8 MHz-klok van de ST (HSE-bypass, PLL ×8 = 64 MHz → SPI exact 8 en 32 MHz). Dat vraagt een hardwarewijziging.

## Open punten / mogelijke vervolgstappen
- Flashen en testen in kleur 50 Hz, 60 Hz en mono; daarna V.Off-waarden opnieuw afstellen.
- OSD verschijnt bij elke floppyactiviteit: Frank probeert eerst FF.CFG
  (`display-on-activity = no`, `display-off-secs = 5`, `osd-display-order = 0`). Nadeel: het OLED
  gaat dan ook uit. Alternatief (nog niet gebouwd): de firmware toont de OSD alleen als de image-naam
  verandert, een paar seconden lang, ongeacht wat FlashFloppy met de backlight doet.
- Ctrl+Alt+Delete roept `system_reset()` aan: dat reset de STM32, niet de ST (TOS 1.04+ reset
  zichzelf wel bij Ctrl+Alt+Del). Is nog niet besproken.
- B8/B9 (BTN_LEFT/RIGHT op JP5) worden in `main()` als open-drain LOW uitgang gezet. Nog niet nagekeken of
  dat bedoeld is.

## Codestijl
Linux/Keir-stijl: 4 spaties inspringen, geen tabs (atari.c had tabs; die zijn opgeschoond),
C99 met `-Wdeclaration-after-statement` (declaraties bovenaan een blok), commentaar in het Engels.
