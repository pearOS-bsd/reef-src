# bash all.sh : on the host: all groups, each in its own folder of the container, some at a time
cd ~/.pear/tmp/tk/app/fonts
E='-e T=900 -e APPFONT1=/tmp/fonts/apphack.ttf -e APPFONT2=/tmp/fonts/app0.ttf -e GONE=/tmp/fonts/gone'
D='-e DRIVER=/tmp/fonts/driver5 -e PRE=/tmp/fonts/fake5.so'
M='-e CONF=/tk/app/fonts/test/made.conf -e FONT_ROOTS=/tk/app/fonts/made'
{
for g in ${GROUPS_:-features nulls files match list names register reread fallback vary varied many data draw cache shape}; do echo "$g|$E $D|$g"; done
for g in files names data list fuzz-draw-9-1500 fuzz-shape-9-1500 fuzz-match-9-600; do echo "made-$g|$E $D $M|$g"; done
echo "made-sizes|$E $D $M -e PRE=/tmp/fonts/fake5.so:/tmp/fonts/pad.so|sizes"
echo "gone|$E $D -e CONF=/tk/app/fonts/test/gone.conf|gone"
echo "scan|$E $D -e CONF=/tk/app/fonts/test/scan.conf|names list match fallback"
for t in "Lato" "Made m-noeight" "Made m-zeroeight" "Nonexistent"; do n=$(echo "$t" | tr -d ' '); echo "theme-$n|$E $D $M -e THEME_FAMILY=$(echo "$t" | sed 's/ /\\\\ /')|match"; done
for g in fuzz-draw-31-2500 fuzz-odd-31-300 fuzz-shape-31-5000 fuzz-match-31-2000 fuzz-data-31-600; do echo "$g|$E $D|$g"; done
} | xargs -P ${J:-2} -d '\n' -I{} sh -c 'IFS="|"; set -- $(printf "%s" "{}"); n=$1; e=$2; g=$3; unset IFS; eval docker exec -e NAME=all-$n $e rb bash /tk/app/fonts/test/run.sh $g 2>&1 | cut -c1-200 | sed "s/^/$n: /"'
