# Tests of single units of the AppKit host

The differential tests the units were written with: the published library and ours get the same calls (with fakes of the system
libraries, devices and buses), the logs must be the same. They are run by hand: each folder has a README, NOTE or HOWTO, and its
scripts expect it at `/tk/app/<unit>/test` (`text` at `/tk/app/text`), the repository at `/w`, the package's `usr` at `/o`, the toolkit's
libraries at `/root2` and its source at `/tk/tk7` (mount `DIR:/tk/app/<unit>/test:ro` and so on). What they cover is in
`docs/APPKIT-HOST.md`. `video` makes its streams with `mkstreams.sh` (ffmpeg); `fonts` and `text` use fonts of the system.
The drivers one folder up (`run.sh`) need nothing of this and run in CI.
