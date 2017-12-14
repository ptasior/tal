#!/bin/env bash

# https://stackoverflow.com/questions/7998302/graphing-a-processs-memory-usage

# trap ctrl-c and call ctrl_c()

if [ "$1" == "" ];
then
	echo "Missing program name";
	exit -1;
fi

trap ctrl_c INT

LOG=$(mktemp)
SCRIPT=$(mktemp)
IMAGE=$(mktemp)

echo "Output to LOG=$LOG and SCRIPT=$SCRIPT and IMAGE=$IMAGE"


cat >$SCRIPT <<EOL
set term png small size 800,600
set output "$IMAGE"

set ylabel "VSZ"
set y2label "%MEM"

set ytics nomirror
set y2tics nomirror in

set yrange [0:*]
set y2range [0:*]

plot "$LOG" using 3 with lines axes x1y1 title "VSZ", \
     "$LOG" using 2 with lines axes x1y2 title "%MEM"
EOL


function ctrl_c()
{
	rm $SCRIPT
	rm $LOG
	echo "Image is saved in $IMAGE"
	exit 0;
}

ps -C $1 -o pid=,%mem=,vsz= | tee -a $LOG
gnuplot $SCRIPT
viewnior $IMAGE&

while true; do
	ps -C $1 -o pid=,%mem=,vsz= | tee -a $LOG
	gnuplot $SCRIPT
	sleep 2
done

