# OSD-instellingen: wanneer en wat FFXtreme toont

## Hoe de OSD aan zijn informatie komt

De FFXtreme-print hangt aan de I2C-displaybus van de Gotek (SCL/SDA, kabel JP5).
Voor FlashFloppy is het een display:

- **A0-A1 niet verbonden:** de OSD bootst een 16×2 HD44780-LCD met PCF8574-backpack na
  (I2C-adres 0x27). Alleen tekst, communicatie in één richting.
- **A0-A1 verbonden (zo is het op de FFXtreme-print, ROW2 pin 15–16):** eigen
  FF OSD-protocol (I2C-adres 0x10, FlashFloppy v3.4a of nieuwer). Werkt in twee richtingen:
  FlashFloppy leest ook de knopstatus terug. Daardoor werken Ctrl+Alt+← / → / ↑ op het
  ST-toetsenbord als de knoppen van de Gotek.

Een OLED op JP3 zit op dezelfde bus en werkt tegelijk, op zijn eigen adres.

Wanneer de OSD verschijnt, beslist FlashFloppy: de OSD is zichtbaar zolang FlashFloppy de
"backlight" aan heeft staan. Het in- en uitschakelen regel je dus in **FF.CFG** op de USB-stick.

## FF.CFG: alleen de titel tonen, niet bij elke floppyactiviteit

```
# Niet meer aangaan bij track-wissels, schrijfacties of drive select
display-on-activity = no

# Na 5 seconden zonder activiteit weer uit (0 = altijd uit, 255 = altijd aan)
display-off-secs = 5

# OSD alleen de image-naam laten tonen, geen status (track/side)
osd-display-order = 0
```

### display-on-activity

| Waarde | Display gaat aan bij |
|---|---|
| `yes` | track-wissels en schrijfacties |
| `sel` | drive select |
| `no`  | geen automatische trigger |

Met `no` gaat het display nog wel aan als je met de knoppen (Ctrl+Alt+← / →) een andere
image kiest.

### display-off-secs

Na hoeveel seconden zonder activiteit het display uitgaat (0–255). `0` = altijd uit,
`255` = altijd aan.

**Let op:** dit geldt voor álle displays, dus ook het OLED op JP3 gaat dan uit.

### osd-display-order (OSD) en display-order (LCD/OLED)

Rijen van boven naar beneden, gescheiden door komma's. Elke rij is een cijfer, eventueel
met `d` erachter voor dubbele hoogte.

| Code | Inhoud |
|---|---|
| `0` | Huidige image-naam |
| `1` | Status (track/side) |
| `2` | Image/volume-info |
| `3` | Huidige submap |
| `7` | Lege rij |

Voorbeelden:
- `osd-display-order = 0`: alleen de titel
- `osd-display-order = 3,0`: map en titel
- `osd-display-order = 0d`: titel in dubbele hoogte

`display-order` werkt hetzelfde, maar voor het OLED, en staat los van de OSD. Het OLED kan
dus de track-info blijven tonen.

`osd-columns` (16–40) stelt het aantal tekstkolommen van de OSD in, maar alleen als er
geen LCD/OLED gevonden wordt.

## Alternatief als het OLED aan moet blijven

Als `display-off-secs` ongewenst is omdat het OLED dan ook uitgaat, kan de FFXtreme-firmware
het zelf regelen: de OSD alleen tonen als de image-naam verandert, een paar seconden laten
staan en dan verbergen, ongeacht wat FlashFloppy met de backlight doet. Nog niet gebouwd.

## OSD-instelmenu (Atari-toetsenbord)

Alle sneltoetsen alleen met **Ctrl+Alt** ingedrukt:

| Toets | Functie |
|---|---|
| Help | Instelmenu openen |
| ↑ | Volgende menu-item |
| ← / → | Waarde aanpassen (ingedrukt houden herhaalt) |
| Undo | Menu verlaten zonder op te slaan |
| O | OSD aan/uit |
| B | Opstarten van interne/externe drive (reset de ST, wordt onthouden) |
| S | Mono/stereo geluid (wordt onthouden) |
| F1–F4 | TOS kiezen (reset de ST, wordt onthouden) |
| Delete | Reset |

Na 1 minuut zonder toetsaanslag sluit het menu vanzelf, ook zonder op te slaan.

Volgorde van het menu:
1. Sync Polarity (Low laten staan)
2. Pixel Timing
3. Display Height
4. Display Output
5. Display Enable
6. Color H.Off
7. Color V.Off (50 Hz)
8. 60Hz V.Off
9. Mono H.Off
10. Mono V.Off (in stappen van 2 lijnen)
11. TOS rom
12. Sound
13. Boot
14. Save New Config?: kies met ←/→ tussen Save, Save+Reset, Use, Discard en Factory
    Reset, en bevestig met ↑.

Ook te bedienen via de seriële poort (Prog-header JP4, 115200 baud): spatie = volgende,
`o` = links, `p` = rechts.
