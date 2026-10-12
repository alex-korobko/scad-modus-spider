#!/bin/sh
# Cycle frames into ../4.txt (the only file the emulator reads), then restore the baseline.
# usage: cycle_responses.sh [cycles]   (default 3)
cd "$(dirname "$0")"
N=${1:-3}
i=0
while [ $i -lt $N ]; do
    for v in stop start running coasting stop; do
        echo "cycle $((i+1)): 4_$v.txt"
        cp "4_$v.txt" ../4.txt
        sleep 5
    done
    i=$((i+1))
done
cp ../4_baseline.txt ../4.txt
echo "Restored baseline."
