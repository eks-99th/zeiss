Sender
sx --xmodem -vv file_to_send.txt < /dev/ttyUSB0 > /dev/ttyUSB0
Receiver
rx --xmodem -vv received_file.txt < /dev/ttyUSB1 > /dev/ttyUSB1


https://www.adontec.com/Extended_XMODEM.pdf

# Constrains

- Work only over serial
- Is capable only to send, not to receive