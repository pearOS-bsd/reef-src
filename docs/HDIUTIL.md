# reef-hdiutil

`lib/cutefish-reef/usr/libexec/reef-hdiutil`, called by the `hdiutil` script of the frameworks. Commands: `makehybrid`, `create`,
`convert`, `verify`. Needs ICU (NFD for HFS+ names), liblzma, zlib; bzip2 is opened with `dlopen` (`libbz2.so.1.0`, `libbz2.so.1`).

## Source files and the binary

| file | binary | what |
|---|---|---|
| `main.cpp` | `0xce40`-`0xfb10`, `main` and the static set of options in `.text.startup` | options (`Arguments`: positional, valued options in a fixed set, flags), progress for `-puppetstrings`, temporary file beside the output, rename at the end |
| `tree.cpp` | `0x10160`-`0x14e10` | `scanFolder` (files, links and folders; other kinds left out), `copyFile` (1 MiB at a time), UTF-8 to UTF-16, CRC32 |
| `hfs.cpp` | `0x15b80`-`0x23f90` | HFS+ |
| `optical.cpp` | `0x24700`-`0x34ed0` | ISO 9660, Joliet, UDF 1.02 (`makehybrid`) |
| `udif.cpp` | `0x36380`-`0x43e40`, namespace `diskimage` | UDIF images: formats `UDRO UDZO UDBZ ULFO ULMO UDTO UDRW` (0 to 6), convert and verify |
| `upstream/lzfse` | `0x453d0`-`0x4ba20` | LZFSE from github.com/lzfse/lzfse `e634ca5`, unchanged: its 21 functions match |

`Entry` (0xb0 bytes): type (0 folder, 1 file, 2 link), name, path, size, mode, uid, gid, atime, mtime, ctime, link target, children.

## HFS+ (`makehybrid -hfs`, and `create` before the conversion)

No journal; attributes `kHFSVolumeUnmountedMask`, last mounted version `10.0`, blocks of 4096, volume UUID from `getrandom`
in `finderInfo[6..7]` (`0x5a..` when it fails). Layout: header at 1024 and 1024 before the end, the allocation file from
block 1, the extents file (one empty header node of 4096) after it, the catalog (nodes of 8192) after that, then the data of
every file and link in one extent each, in the order of the walk. Size: `2*catalog nodes + 3 + data blocks`, plus 16 and 1 %,
at least 256 blocks, plus the allocation file. File and folder numbers: root 2, the others from 16 in the order of the walk
(`readdir` order, parent before children).

Catalog: keys compared with Apple's case folding table (TN1150; two tables in the binary, `src/hdiutil/lowercase.h`); names
decomposed with ICU NFD except U+2000-U+2FFF, U+F900-U+FAFF, U+2F800-U+2FAFF; `:` stored as `/`. Records packed in key order
into leaves while `free + size + 2*(records + 2) <= 8192`; index levels above, nodes of one level linked. Dates: created =
min(ctime, mtime), content = mtime, attributes = ctime, accessed = atime. Two names of a folder equal under the folding: error
"... only in case, which this file system cannot keep apart". Images made by ours and the original are the same except the
dates of the volume header and the UUID (`tests`: folders of 8000 files, three B-tree levels, Unicode names, links).

## Optical (`makehybrid`, default ISO 9660 + Joliet + UDF)

Observed on images of the original: ISO PVD 16, Joliet SVD 17, terminator 18, UDF VRS 19-21 (`BEA01 NSR02 TEA01`), main VDS
32-37 (PVD, IUVD, PD, LVD, USD, TD), reserve VDS 48, LVID 64, anchor 256 and in the last sector, partition from 257 to the end
of the data: FSD 0, TD 1, file entries from 2 (root first), directory data of UDF, then the ISO directories, the ISO path
tables (L, M), the Joliet directories and path tables, then the file data, shared by the three file systems. Links are left out
of ISO and Joliet; UDF keeps them (file type 12, path components). Volume ids: ISO upper case, Joliet and UDF as given;
implementation id `*reef`; UDF volume set id `<8 hex digits>` + volume name.
