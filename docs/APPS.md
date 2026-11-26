# Mac apps with the original package and with ours (2026-11-26)

The apps of the published compatibility list, run with the original package and with the 12 files built here (two containers,
headless sway compositor, grim screenshots; each app after a warm-up, twice with the original and once with ours, then more
runs where the original itself varied). Compared: exit status, stderr, `--list-missing` JSON, window titles and sizes, files
written, and pixels. Only start-up and the first screens were compared: no clicks, menus or files yet.

| App | Version | Result |
|---|---|---|
| CotEditor | 7.1.2 | same, pixel for pixel |
| NetNewsWire | 7.2.1 | same |
| MyGo Music | 0.1.25 | same (the time to the first window varies with the network update check in both) |
| Ghostty | 1.4.0 | same (the containers need one host name: the shell prompt shows it) |
| Keka | 1.6.8 | same |
| IINA | 1.5.1 | same |
| Itsycal | 0.15.15 | same (menu bar app, no window) |
| Sloth | 3.6 | same as one of the original's two outcomes (a popup's height varies between runs of the original) |
| Telegram | 13.2.0 | same: the shield button's shadow takes one of two states per run in the original and in ours alike; the drawing calls for it are identical (gdb trace) |
| OmniFocus | 4.10.1 (4.9.3 is no longer offered) | same as one of the original's outcomes (popups vary between runs of the original) |
| Speedtest | - | not tested: Mac App Store only |
| Microsoft Word | - | not tested |

Apple's own apps (for example Calculator) need Apple's private frameworks (Calculate, CalculateUI, TextInputUI ...): they stop
with "library not loaded" with the original package too.
