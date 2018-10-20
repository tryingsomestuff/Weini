#!/bin/bash
outfile=tuning_fen_$(date +"%Y_%m_%d"_%H%M%S).out
date > $outfile
n=64000
for i in `seq 1 $n`; do
    echo "Playing game $i / $n"
    echo "Begin $i" >> $outfile
    ./bin/weini -t "playAI :start: 40" -o "st 25" | grep "FEN :\|Result ::" >> $outfile 2>&1
    echo "End $i" >> $outfile
done
