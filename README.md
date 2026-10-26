# FF Xtreme: On Screen Display for FlashFloppy with adapted hardware and firmware for the Atari ST

## New in v2.0: much improved firmware
- **Rock-steady OSD:** the vertical jumping of the OSD on the Atari ST is fixed, in colour (50 and 60 Hz) and monochrome.
- **Separate vertical offset for 60 Hz** (menu item "60Hz V.Off"), and a clean restart when you switch between a colour and a monochrome monitor.
- **Reliable keyboard control:** mouse and joystick movement can no longer trigger hotkeys, keys no longer get stuck, and hotkeys really need Ctrl **and** Alt.
- **Your choices are remembered:** TOS (Ctrl+Alt+F1–F4), boot drive (Ctrl+Alt+B) and mono/stereo sound (Ctrl+Alt+S) survive a power cycle. EmuTOS, which needs a cold start, now just works.
- **Ctrl+Alt+Delete** resets the Atari ST without restarting the OSD.
- **OSD menu:** leave it with Ctrl+Alt+Undo; it also closes by itself after one minute without key presses.
- Many smaller fixes; see the [release notes](RELEASE_NOTES).

**Upgrading from v1.9:** your OSD settings are reset to the factory defaults on the first start. Note your H.Off/V.Off values before you upgrade.

## Recommended FlashFloppy settings (FF.CFG)
When and what the OSD shows is decided by FlashFloppy, so it is set in `FF.CFG` on your USB stick.
These two settings make the OSD much less intrusive:

```
display-off-secs = 5
osd-display-order = 0d,1
```

- `display-off-secs = 5`: the OSD disappears after 5 seconds instead of staying on screen for a
  minute (the FlashFloppy default is 60). Note that this also switches off an OLED display on the Gotek.
- `osd-display-order = 0d,1`: show the image name in double height plus the status line (track,
  side). With the default, an OLED-equipped Gotek also sends the current subfolder as a first row,
  which is empty in the root of the stick and shows as a thick empty band above the text.

FlashFloppy reads `FF.CFG` only at startup, so restart after changing it.

## Original project page
This project is a extended version of Keir Fraser [**FlashFloppy**](https://github.com/keirf/flashfloppy) and [**FF OSD**](https://github.com/keirf/flashfloppy-osd).
You should take a look at his work first!

## About this project
Keirfs On Screen Display (OSD) doesn´t support a keyboard controller for the Atari ST while this version does.
I have added some extra features to the firmware as well:
- Send FlashFloppy output to your Atari ST video display in all three resolutions, monochrome included.
- Control FlashFloppy via your Atari keyboard (Optional but prefered)
- Reset your Atari ST from the keyboard (Optional)
- Toggle intern and extern floppy to be disk A/B so that you are able to boot from your extern floppy drive.
- Switch your TOS roms (optional + extra hardware)
- Switch between mono and stereo sound (optional + extra hardware)

FF OSD also emulates the popular LCD I2C interface, allowing
connection to a wider range of hardware which traditionally uses an LCD.

## Download
- [**Download FF Xtreme**](https://github.com/RetroLoft/FFXtreme/wiki/downloads)

## Documentation
- [**Read the GitHub Wiki**](https://github.com/RetroLoft/FFXtreme/wiki)

## Demo video
[![youtube](https://user-images.githubusercontent.com/2814747/211155171-bc639b24-a388-4f82-a549-2daf706933e2.jpg)](https://youtu.be/2rItDM706Do)

## Redistribution

FF OSD source code, and all binary releases, are freely redistributable
in any form. Please see the [license](COPYING).

[FF]: https://github.com/keirf/flashfloppy/wiki
[Downloads]: https://github.com/keirf/flashfloppy-osd/wiki/Downloads
