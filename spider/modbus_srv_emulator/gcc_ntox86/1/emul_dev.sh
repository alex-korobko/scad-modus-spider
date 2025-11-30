#!/bin/sh

echo "Running up"
cp 4_running_up.txt 4.txt
echo "Sleeping 10 seconds"
sleep 10
echo "Stopped with block circut"
cp 4_new_msg_stopped.txt 4.txt
echo "Sleeping 10 seconds"
sleep 10
echo "Reset to original state"
cp 4_orig.txt 4.txt
echo "Done"
