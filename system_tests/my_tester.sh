#!/bin/bash

# This script is used to test the serial_example program.
# Run the serial_example program in the background
../build/serial_example --file /home/eks99th/Desktop/zeiss/Send/Hello.txt -p /dev/ttyUSB0 &

# Run the rx command with xmodem protocol
rx --xmodem -vv received_file.txt < /dev/ttyUSB1 > /dev/ttyUSB1
