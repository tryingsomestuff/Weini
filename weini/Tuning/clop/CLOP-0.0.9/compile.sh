#!/bin/bash

cd programs/clop/compgcc
make
cd -
cd programs/clop/compqt/clop-console
qmake && make
cd -
cd programs/clop/compqt/clop-gui
qmake && make
cd -

