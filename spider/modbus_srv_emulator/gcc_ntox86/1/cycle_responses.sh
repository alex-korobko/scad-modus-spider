#!/bin/sh

# Script to cycle through Modbus response files
# Copies 4_1.txt through 4_8.txt to 4.txt with 5 second delays

i=1
while [ $i -le 8 ]; do
    source_file="4_${i}.txt"
    target_file="4.txt"
    
    if [ -f "$source_file" ]; then
        echo "Copying $source_file to $target_file..."
        cp "$source_file" "$target_file"
        echo "Waiting 5 seconds before next iteration..."
        sleep 5
    else
        echo "Warning: $source_file not found, skipping..."
    fi
    i=$((i + 1))
done

echo "Cycle complete."

