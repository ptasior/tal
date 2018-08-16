#!/bin/bash

DIR="$(dirname $(readlink -f $0))"
cd "$DIR"/../..

python3 $DIR/dataGen.py "letters lua_lib shaders" output/letters.game

