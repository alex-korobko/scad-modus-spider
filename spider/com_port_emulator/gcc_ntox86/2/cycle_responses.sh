#!/bin/sh
# Cycle variants into 4.txt, then restore the baseline
cd "$(dirname "$0")"
for v in running_up running_down block_circuit_broken mu brake_fault; do
    echo "Switching to 4_$v.txt"
    cp "4_$v.txt" 4.txt
    sleep 5
done
cp 4_baseline.txt 4.txt
echo "Restored baseline."
