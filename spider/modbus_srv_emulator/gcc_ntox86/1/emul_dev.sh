#!/bin/sh

echo "New msg"
cp 4_new_msg.txt 4.txt
echo "Sleeping 10 seconds"
sleep 10
echo "Running and adding a new msg"
cp 4_running.txt 4.txt
echo "Sleeping 10 seconds"
sleep 10
echo "Stopped and added a new msg"
cp 4_new_msg_stopped.txt 4.txt
echo "Done"
