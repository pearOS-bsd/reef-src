# reef-diagnose

`bin/reef-diagnose [report file]` writes a report on the graphics of the machine (default `~/reef-diagnose.txt`), without the names
of the machine and of the user. It needs the toolkit (`EmbeddedWindow` draws the glass), EGL and GLESv2.

## Source files and the binary

| file | binary | what |
|---|---|---|
| `main.cpp` | `main` in `.text.startup` | options, the order of the sections, the probe of reef, the head of the report |
| `report.cpp` | `0x16ef0`-`0x1c990` | `Report` (text, problems, counts), `run`, files, `format`, the report without names |
| `activity.cpp` | `0x1ecc0`-`0x207b0` | the GPU activity log of reef's programs and the GPU hangs of the kernel log |
| `system.cpp` | `0x38610`-`0x42010` | system, packages, GPU devices, kernel log, `ClockSampler`, resets, eglinfo |
| `egl.cpp` | `0x441d0`-`0x4b7d0` | the EGL display, contexts, the OpenGL entry points, the section on EGL and OpenGL |
| `driver.cpp` | `0x4ea40`-`0x58770` | checks of the driver without reef: DMA-BUF export read by another process (`--import-check`), what new textures and buffers have in them |
| `glass.cpp` | `0x59740`-`0x601f0` | how fast the toolkit draws glass |

`main` and `report.cpp` share copies of `operator+` in the binary, so the original has them in one file.

## The report

- Order: System, Packages, GPU devices, Kernel log, GPU work at each hang, GPU activity log, EGL and OpenGL, Driver checks, reef GPU
  probe, Glass, GPU clocks, GPU resets, eglinfo. Sections `== title ==`; fields `name:` padded to 28 columns; results
  `PASS|FAIL|WARN|INFO|SKIP check: message`. FAIL and WARN are counted and listed at the start.
- Names: the host name, the user's name and full name as whole words (not beside a letter, a digit, `_` or `-`), each longer than 2,
  become `<host>` and `<user>`; the home folder becomes `~`.
- Kernel log: `journalctl -k -b -o short-monotonic --no-pager -q`, the lines of GPU drivers, the first 80 and last 220 when there are
  more than 300. Hangs (`short-iso-precise`): i915 `NAME[THREAD] context reset due to GPU hang`, amdgpu
  `Process information: process NAME pid N`. A logged work belongs to a hang when its thread or program is the hang's and its time
  reaches within half a second of it; the named one is the longest of the thread the kernel names, else the longest.
- Activity log: `~/.cache/cutefish-reef/gpu-activity.log` (and `.old`):
  `TIME PROGRAM pid N tid N SLOW|RUNNING WORK for N ms: ... | also running: ...`.
- Probe: `reef <usr/libexec/reef-gpu-probe> -ReefHeadless YES`, 240 seconds; lines `RUN test`, `DONE ...`, results, others as `  | line`.
- Clocks: ten samples a second of `gt_act_freq_mhz`, `device/tile0/gt0/freq0/act_freq`, `device/gpu_busy_percent`,
  `device/pp_dpm_sclk` and `gt/gt0/throttle_reason_*` of each card.
- Glass: a window of the mode of the first connected screen, 40 frames of 0, 5, 6 and 7 panes over moving content, on a surfaceless
  OpenGL ES context (robust when the driver makes one); the first four frames are not in the median.

## Checked

`tests/diagnose.py`: 20 cases on made up machines (a preloaded library gives `/sys`, `/proc`, `/etc` from a folder; `journalctl`,
`dpkg-query`, `eglinfo` and `reef` are scripts). Checked once by hand (2026-11-25): the panes given to `View::setGlass` and the
pixels of the textures drawn (16 frames, llvmpipe) are the same as the original's; the whole report in the container is the same
but times. Not run: a real GPU (DMA-BUF export works on none here), a lost context, the 240 seconds of the probe.
