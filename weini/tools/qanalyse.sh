#!/bin/bash
./bin/weini -t "analysis :$1: 0" | grep "FEN :\|Best move is"
