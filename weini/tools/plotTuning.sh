#!/bin/bash

rm -rf tmp.tuning
cat Tuning/test.dat | grep ^S | tr 'S' ' ' > tmp.tuning

python plotTuning.py




