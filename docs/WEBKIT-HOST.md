# libreef-webkit-host.so
`reef_webkit_host_interface(1)` returns a table: a version word and 100 function pointers (index in the comments of `src/webkit-host`).
Any other version gives null. It is written on the WebKit C API of cutefish-webkit 2.54.2; only the interface function is exported.

| file | table entries |
|---|---|
| runtime.cpp | 1-3, 58, 80, 81, 96, 97 |
| content.cpp | 4, 6, 7, 9, 70-74, 76, 77, 85 |
| page.cpp | 20-40, 50, 56, 67, 69, 78, 79, 87, 92, 93, 99, 100 |
| data.cpp | 11-13, 15-17 |
| dialogs.cpp | 55, 57, 59 |
| input.cpp | 44-48 |
| events.cpp, view.cpp, viewclient.cpp | callbacks of WebKit |

The program's callbacks (`ViewClient`) by offset: 0x08 cursor, 0x10 tool tip, 0x18 property changed (1 title, 2 URL, 3 loading, 4 progress,
5 back, 6 forward, 7 secure content), 0x20 navigation event (1 started, 2 redirected, 3 committed, 4 finished, 5 failed early, 6 failed,
7 document loaded), 0x40 close, 0x60 web process ended, 0x68 input method, 0x88 hide popup, 0xa0 download, 0xb0 find result,
0xb8 contents size, 0xc0 wheel not handled, 0xc8 page colour, 0xd8 / 0xe0 full screen, 0xf0 drag.

The argument of user scripts is "at document start" (1), not "at the end": the comparison showed the inverted test.
No behaviour test yet. CI does not build this target (the WebKit package is not in the private release).
