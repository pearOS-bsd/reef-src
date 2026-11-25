# bash fuzz.sh SEED... : on the host: random calls with each seed
cd ~/.pear/tmp/tk/app/fonts
for s in "$@"; do for k in draw-$s-${ND:-3000} odd-$s-${NO:-400} shape-$s-${NS:-8000} match-$s-${NM:-3000} data-$s-${NX:-1500}; do echo $k; done; done | xargs -P ${J:-4} -I{} sh -c 'docker exec -e NAME=f-{} -e T=1800 -e OURS='${OURS:-/tmp/fonts/ours.so}' -e DRIVER=/tmp/fonts/driver4 -e PRE=/tmp/fonts/fake3.so rb bash /tk/app/fonts/test/run.sh fuzz-{} 2>&1 | cut -c1-260 | sed "s/^/{}: /"'
