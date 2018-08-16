#!/bin/bash
./bin/weini -analysis "$1" 0 | grep "FEN :\|Best move is"
