#!/bin/bash
for i in $(cat $1 | grep "FEN :" | awk -F":" '{print $NF}' | tr ' ' '_' ) ; do 
   echo $i
   ./tools/qanalyse.sh "$(echo $i | tr '_' ' ')" ; 
done
