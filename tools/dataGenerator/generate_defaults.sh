#!/bin/bash

DIR="$(dirname $(readlink -f $0))"
cd "$DIR"/../../assets

python3 $DIR/dir2gamefile.py "defaults" ../output/defaults.game
python3 $DIR/bin2cpp.py ../output/defaults.game ../output/embedded_defaults.h defaultFiles

