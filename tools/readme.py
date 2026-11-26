#!/usr/bin/env python3
"""Put the numbers of total.py into the README.  usage: total.py ... | readme.py README.md DATE
Rows of the table and the paragraph of the total are found by the names of the files; a file that was not built keeps its row."""
import re, sys

path, date = sys.argv[1:3]
text = open(path).read()
original = text
for line in sys.stdin:
    line = line.rstrip('\n')
    print(line)
    row = re.match(r'(\S+): (\d+)/(\d+) functions, (\d+)/(\d+) bytes = ([\d.]+)%', line)
    written = re.match(r'written: (\d+)/(\d+) functions', line)
    macho = re.match(r'macho: (\d+)/(\d+) files rebuilt, (\d+)/(\d+) functions, (\d+)/(\d+) bytes', line)
    total = re.match(r'total: (\d+)/(\d+) functions, (\d+)/(\d+) bytes = ([\d.]+)%', line)
    if macho:
        files, allfiles, same, functions, bytes_, size = (int(macho[i]) for i in range(1, 7))
        text = re.sub(r'\*\*Mach-O files rebuilt: \d+ of \d+ files; [\d.]+ % of the code bytes \(\d+ of \d+\) and [\d.]+ % of the functions \(\d+ of \d+\) are the\s+same as in the published files\*\* \([\d-]+\)',
                      '**Mach-O files rebuilt: %d of %d files; %.1f %% of the code bytes (%d of %d) and %.1f %% of the functions (%d of %d) are the\nsame as in the published files** (%s)'
                      % (files, allfiles, 100 * bytes_ / size, bytes_, size, 100 * same / functions, same, functions, date), text)
    elif total:
        same, functions, bytes_, size = (int(total[i]) for i in range(1, 5))
        text = re.sub(r'\*\*Total of the 12 ELF files: [\d.]+ % of the code bytes \(\d+ of \d+\) and [\d.]+ % of the functions \(\d+ of \d+\) are the\s+same as in the published binaries\*\* \([\d-]+\)',
                      '**Total of the 12 ELF files: %.1f %% of the code bytes (%d of %d) and %.1f %% of the functions (%d of %d) are the\nsame as in the published binaries** (%s)'
                      % (100 * bytes_ / size, bytes_, size, 100 * same / functions, same, functions, date), text)
    elif written:
        done, functions = int(written[1]), int(written[2])
        text = re.sub(r'Written: [\d.]+ % of the functions \(\d+ of \d+\)', 'Written: %.1f %% of the functions (%d of %d)' % (100 * done / functions, done, functions), text)
    elif row:
        name, same, functions, bytes_, size, percent = row[1], int(row[2]), int(row[3]), int(row[4]), int(row[5]), float(row[6])
        numbers = '%d/%d functions, %d/%d bytes' % (same, functions, bytes_, size) if bytes_ == size \
            else '%d/%d functions, %.1f %% of bytes' % (same, functions, percent)
        text = re.sub(r'(\| `%s`(?: \(\w+\))? \| )\d+/\d+ functions, (?:\d+/\d+ bytes|[\d.]+ %% of bytes)' % re.escape(name), r'\g<1>' + numbers, text)
if text != original:
    open(path, 'w').write(text)
    print(path + ': updated')
