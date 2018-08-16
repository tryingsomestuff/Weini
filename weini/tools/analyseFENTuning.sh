
for i in $(cat $1 | grep "FEN :" | awk -F":" '{print $NF}' | tr ' ' '_' ) ; do ./tools/qanalyse.sh "$(echo $i | tr '_' ' ')" ; done
