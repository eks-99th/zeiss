#!/bin/bash

bash -c "python3 ./receiver.py" &
SCRIPT_PID=$!
echo "receiver script is running in the background (PID: $SCRIPT_PID)."
echo "Running serial_example..."
../build/serial_example --file ../Send/Hello.txt -p /dev/ttyUSB0
wait $SCRIPT_PID