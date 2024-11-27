# ESX: Minimal Implementation of the XMODEM Protocol

ESX (Erick's SX) is a lightweight implementation of the XMODEM protocol. This 
project implements the sender functionality of XMODEM, inspired by the sx tool 
on Linux.

## Features

- Sends files over serial communication using the XMODEM protocol.
- Designed to interoperate with tools like rx on Linux.

## Limitations

- Send-only: Currently supports only sending files, not receiving.
- Serial-only: Works exclusively over serial connections, but it is easy to 
extend.
- Fixed Parameters: Serial communication settings are hardcoded.


## Building the Project

1. Create a build directory:
```bash
mkdir build
cd build
```
2. Generate the build files with CMake:
```bash
cmake ..
```
3. Compile the project:
```bash
    make
```
## Running Unit Tests

1. Ensure the project is built (see Building the Project).
2. Navigate to the build directory and execute the tests:
```bash
    cd build
    ctest
```

## Running System Tests
### Prerequisites
- Connect two serial devices to your computer (e.g., ttyUSB0 and ttyUSB1).
- Verify the devices can communicate with each other.

### Setup

1. Open system_tests/my_tester.sh and modify the following:
    - Replace /path/to/file with the file you want to send.
    - Replace /dev/ttyUSB0 and /dev/ttyUSB1 with the serial devices connected to your system.

2. Run the helper script my_tester.sh


## Example Workflow

Here’s an example of sending a file named example.txt:

1. Connect two serial devices:
        Sender: /dev/ttyUSB0
        Receiver: /dev/ttyUSB1

2. Run the sender:

../build/esx --file example.txt -p /dev/ttyUSB0 &

3. Run the receiver:

stty -F /dev/ttyUSB1 115200 cs8 -cstopb -parenb
rx --xmodem -vv received_file.txt < /dev/ttyUSB1 > /dev/ttyUSB1

4. Verify the file received_file.txt

![Demo](Demo.gif)