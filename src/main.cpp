#include <LibSerial/SerialPort.h>
#include <iostream>

using namespace LibSerial;

int main() {
  SerialPort serial;

  try {
    serial.Open("/dev/ttyUSB0");
    serial.SetBaudRate(BaudRate::BAUD_9600);
    serial.SetCharacterSize(CharacterSize::CHAR_SIZE_8);
    serial.SetStopBits(StopBits::STOP_BITS_1);
    serial.SetParity(Parity::PARITY_NONE);

    // Write data
    serial.Write("Hello, Serial!\n");

    // Read data
    std::string data;
    serial.ReadLine(data, '\n', 1000); // Read until newline or timeout
    std::cout << "Received: " << data << std::endl;

    serial.Close();
  } catch (const OpenFailed &) {
    std::cerr << "Failed to open serial port!" << std::endl;
  }

  return 0;
}
