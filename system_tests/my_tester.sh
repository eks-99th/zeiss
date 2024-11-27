#!/bin/bash

# Run the esx program in the background
../build/esx --file /home/eks99th/Desktop/zeiss/tests/test_data/big.txt -p /dev/ttyUSB0 &

# Run the rx command with xmodem protocol
stty -F /dev/ttyUSB1 115200 cs8 -cstopb -parenb
rx --xmodem -vv received_file.txt < /dev/ttyUSB1 > /dev/ttyUSB1
