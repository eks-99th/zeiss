import serial

# Configure the serial port
NAK = b'\x15'
ser = serial.Serial(
    port='/dev/ttyUSB1',      # Specify the serial port
    baudrate=115200,          # Set baud rate to 115200
    bytesize=serial.EIGHTBITS,  # 8 data bits
    parity=serial.PARITY_NONE,  # No parity
    stopbits=serial.STOPBITS_ONE,  # 1 stop bit
    timeout=1,                 # Timeout for read operations (in seconds)
    xonxoff=False,             # Disable software flow control
    rtscts=False,              # Disable hardware (RTS/CTS) flow control
    dsrdtr=False               # Disable hardware (DSR/DTR) flow control
)
# Write data to the serial port
data_to_send = [b'A', b'B', b'C', NAK]
# Send each value in the array
for data in data_to_send:
    ser.write(data)

# Close the serial port
ser.close()
