# libreef-webkit-host.so
`reef_webkit_host_interface(1)` returns a table: a version word and 100 function pointers (index in the comments of `src/webkit-host`).
Any other version gives null. It is written on the WebKit C API of cutefish-webkit 2.54.2; only the interface function is exported.

| file | what |
|---|---|
| table.cpp | the exported function and the table |
| runtime.cpp, content.cpp, page.cpp, data.cpp, dialogs.cpp, input.cpp, scheme.cpp, download.cpp, view.cpp | the functions (index in the comments) and their callbacks |
| events.cpp, viewclient.cpp | callbacks of WebKit |
| text.cpp, json.cpp | strings and JSON |

Build facts found by comparison: no `-fvisibility-inlines-hidden` (the original exports the same weak libstdc++ symbols);
hidden visibility does not reach `extern` declarations, so what another file defines goes through the GOT (this tells which
variables and functions share a file); `g_tasks` and the sandbox path are initialised in one file.

The program's callbacks (`ViewClient`) by offset: 0x08 cursor, 0x10 tool tip, 0x18 property changed (1 title, 2 URL, 3 loading, 4 progress,
5 back, 6 forward, 7 secure content), 0x20 navigation event (1 started, 2 redirected, 3 committed, 4 finished, 5 failed early, 6 failed,
7 document loaded), 0x40 close, 0x60 web process ended, 0x68 input method, 0x88 hide popup, 0xa0 download, 0xb0 find result,
0xb8 contents size, 0xc0 wheel not handled, 0xc8 page colour, 0xd8 / 0xe0 full screen, 0xf0 drag.

The argument of user scripts is "at document start" (1), not "at the end": the comparison showed the inverted test.
All 100 entries are written and the library links with nothing unresolved. `tests/webkit.py` runs the original and ours with a
stand-in for WebKit (JSON, input, data kinds, find, screens, application names): no difference in 9988 lines. Not covered by it:
views and their pixels, menus, dialogs, downloads, schemes, real pages. CI does not build this target (the WebKit package is not in
the private release).
