#!/bin/bash
# In the container: synthrun.sh NAME: the cases of synth/, with a table of what they reached.
bash /tk/app/video/test/run.sh $1 --every 3 /tk/app/video/test/${2:-synth}/*.case
cd /out/video/test; python3 /tk/app/video/test/table.py $1.orig.log > $1.table
cut -c1-120 $1.table | awk '{n[substr($1,1,4)]++; d[substr($1,1,4) " " $2]++; for(i=1;i<=NF;i++){if($i=="pics")p[substr($1,1,4)]+=$(i+1); if($i=="failed")f[substr($1,1,4)]+=$(i+1); if($i=="ok")o[substr($1,1,4)]+=$(i+1)}} END {for(k in d)print k,d[k]; for(k in p)print k,"pictures",p[k],"ok",o[k],"failed",f[k]}'
grep -a -E "^(signal|exit [1-9])|exception" $1.orig.log | sort | uniq -c
echo done
