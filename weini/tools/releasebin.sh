#!/bin/bash

dir=$(readlink -f $(dirname $0)/../)

cd $dir

tagname=$1

mkdir -p release
mkdir -p release/weini_$tagname
cd release/weini_$tagname
gccver=$(g++ -v 2>&1 | grep "gcc version" | awk '{print $3}')
cp ../../bin/weini weini_linux_gcc_${gccver}_popcnt_x64_$tagname
cp ../../config.json .
mkdir -p Book
cp ../../Book/book1.pgn Book/
cp ../../Book/book1.bin Book/
cp ../../Book/book3.pgn Book/
cp ../../Book/book3.bin Book/
cp ../../README.md .

cd -
cd release
tar cvzf weini_$tagname.tar.gz weini_$tagname
cd -

rm -rf release/weini_$tagname

