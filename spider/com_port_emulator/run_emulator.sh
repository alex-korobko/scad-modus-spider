#!/bin/sh
# Launch com_port_emulator and log its output to a file.
# Usage: ./run_emulator.sh [port_number] [baud_rate] [log_file] [reply_delay_ms]
# Example: ./run_emulator.sh 2 19200 emulator.log

PORT=${1:-2}
BAUD=${2:-19200}
LOG=${3:-emulator.log}
DELAY=${4:-10}

# Run from the script's directory so ./com_port_emulator and the device
# response directories are found.
DIR=`dirname "$0"`
cd "$DIR" || exit 1

if [ ! -x ./com_port_emulator ]; then
    echo "./com_port_emulator not found or not executable in `pwd`" >&2
    exit 1
fi

echo "Starting com_port_emulator: /dev/ser$PORT, $BAUD baud, log: `pwd`/$LOG"
echo "Stop with Ctrl-C"

# Keep the terminal view and the file in sync; fall back to file only
# if tee is not available.
if command -v tee >/dev/null 2>&1; then
    ./com_port_emulator "$PORT" "$BAUD" "$DELAY" 2>&1 | tee "$LOG"
else
    ./com_port_emulator "$PORT" "$BAUD" "$DELAY" > "$LOG" 2>&1
fi
