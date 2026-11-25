for r in "6bc30 6f5d0" "bed00 cd480"; do python3 -B /tk/app/range.py /o/lib/cutefish-reef/host/libreef-appkit-host.so /tmp/text/app.so $r; done > /tmp/text/range.txt
awk '$1=="="{n++;b+=$3;next} {m++;c+=$2} END{print n" matched, "b" bytes; "m" not, "c" bytes"}' /tmp/text/range.txt; grep -v "^=" /tmp/text/range.txt
