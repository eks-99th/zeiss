#!/bin/bash

bash "receiver.sh" &
SCRIPT_PID=$!
echo "receiver script is running in the background (PID: $SCRIPT_PID)."
echo "Running serial_example..."
../build/serial_example
wait $SCRIPT_PID