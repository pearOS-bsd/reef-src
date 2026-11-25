# libreef-appkit-host.so

`lib/cutefish-reef/host/libreef-appkit-host.so`: everything the Mach-O side of AppKit asks of Linux, over the toolkit. One export,
`reef_appkit_host_interface(version)`: the table (`table.cpp`, `interface.h`: 317 functions) when the version is 24, else null.
`hostInitialize` (entry 0) gets the program's callbacks (`reef_ui_callbacks` in `host.h`); `-ReefHeadless YES` among the program's
arguments makes an application without a display, which the tests use (also `-ReefHeadlessSessionBus`, `-ReefHeadlessStatusItems`,
`-ReefHeadlessDarkMode`, `-ReefHeadlessSound`, `-ReefOutputScaleRendering`, `-ReefOutputScaleComposition`).

## Source files and the binary

| file | binary | what |
|---|---|---|
| `application.cpp` | `0x30c00`-`0x35f00` | start, event loop, theme, screens |
| `blur.cpp` | `0x36ec0`-`0x39b90` | what a window blurs, materials behind it, its shape |
| `clipboard.cpp` | `0x39c30`-`0x3bf70` | clipboard, change count |
| `desktop.cpp` | `0x3bd00`-`0x45bf0` | MIME types, default applications, launching, file manager, the running instance (`org.cutefish.Application`), now playing |
| `drag.cpp` | `0x45c00`-`0x4ad80` | drops, drag sources, dragging windows |
| `dialogs.cpp` | `0x4ad80`-`0x52600` | file chooser |
| `graphics.cpp`, `graphics.h` | `0x52600`-`0x56340` | `reef_ui_gc`: drawing on `ui::Canvas2D` or into a PDF document |
| `images.cpp` | `0x56340`-`0x5f780` | images, DMA-BUF images, decoding, encoding, icons, symbols |
| `input.cpp` | `0x5f780`-`0x600e0` | scrollers, cursors, text input, input for tests |
| `menus.cpp` | `0x60110`-`0x65680` | menus (D-Bus and the toolkit's), status items |
| `paths.cpp` | `0x65680`-`0x67800` | paths, strokes, boolean operations |
| `planes.cpp` | `0x67800`-`0x6bc10` | planes, glass, fading overlays |
| `table.cpp` | `0x6bc10`, data `0x154a60` | the table |
| `text.cpp` | `0x6bc30`-`0x6f5d0` | single lines of text |
| `views.cpp`, `views.h` | `0x6f5d0`-`0x6fcf0`, COMDAT `0x6aeb0`-`0x6bb70` and `0x71b70`-`0x727c0` | `BaseView`, `PlaneView`, `RootView` |
| `windows.cpp` | `0x6fcf0`-`0x71b70` | windows |
| `camera.cpp` `displays.cpp` | `0x727c0` `0x785c0` | V4L2 cameras; vertical blanks of DRM |
| `fade.cpp` | `0x7a900`-`0x7b3f0` | a picture that fades in, drawn by its own thread |
| `fonts.cpp` | `0x7b400`-`0x94180` | faces (fontconfig, FreeType, HarfBuzz), matching, metrics, shaping, fallback, glyph drawing, the match cache on disk |
| `gl.cpp` | `0x94180`-`0x975d0` | OpenGL contexts, surfaces below or above a window, DMA-BUF textures |
| `gpu_activity.cpp` | `0x976e0` | the log of long GPU work (`~/.cache/cutefish-reef/gpu-activity.log`) |
| `jpeg2000.cpp` | `0xa2800` | JPEG 2000 with libopenjp2 |
| `menu_bus.cpp` `now_playing.cpp` `status_item.cpp` | `0xa4440` `0xb45c0` `0x100040` | com.canonical.dbusmenu, MPRIS, org.kde.StatusNotifierItem |
| `paragraph.cpp` | `0xbec70` | paragraphs |
| `pdf.cpp` | `0xcd480`-`0xfc1a0` | `PdfWriter` (PDF 1.7, PDF/A-2b, tagged, encrypted) |
| `sound.cpp` | `0xfc1a0` | sounds (libsndfile, PulseAudio), audio output and input |
| `video*.cpp` | `0x10cc80`-`0x12a320` | H.264, HEVC and VP9 decoding with VA-API |

Globals: `application`, `callbacks`, `headless` at `0x1575c0`-`0x1576xx`. `reef_ui_window` is 0x140 bytes, `reef_ui_gc` 0xe8.

## Checked

- `tests/appkit/run.sh` (CI): `graphics` (random drawing programs and paths, hashes of the pixels), `windows` (random programs of
  windows, planes, glass, input, menus, scrollers without a display, with a clock that is the same in each run: callbacks, answers,
  hashes of the windows), `gl` (contexts, surfaces, frames, DMA-BUF export and import on Mesa's software renderer): the same logs.
  They found one wrong parameter order (`hostScrollerScroll`).
- The units' own tests (`tests/appkit/units`, by hand), all the same as the original: D-Bus (66099 monitored messages), desktop (4682
  lines), drags and dialogs (19749 lines), fonts (70 processes, cache files included), text and paragraphs (28.8 million comparisons,
  PDF included), PDF (2898 documents and 5300 random ones, byte equal), sound, cameras, displays, JPEG 2000, GPU log (over a million
  lines), images and icons (over 100000 checks), video (222115 lines from real streams, 582802 from made up ones, 30000 fuzzed cases).
- Not covered: a real Wayland session (windows on a compositor, subsurfaces, fading overlays, real drags and portals), real GPUs,
  cameras and audio, failing allocations.

## Known

`PdfWriter` and `Canvas2D::restore` of the toolkit do not survive unbalanced groups and saves: the original loops or crashes, so do we.
COLR v0 glyphs drawn aliased read past FreeType's bitmap in the original (the pixels depend on the heap); ours is the same code.
