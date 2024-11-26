// #include "ConcreteSerialPort.hpp"
// #include "Clock.hpp"
#include "IClock.hpp"
#include "exs.hpp"
#include "xmodem_const.hpp"
#include <chrono>
#include <iostream>
#include <libserial/SerialPort.h>

class Clock : public IClock {
public:
  std::chrono::steady_clock::time_point now() const override {
    return std::chrono::steady_clock::now();
  }
};

class exsSerialPort : public Iexs {
public:
  explicit exsSerialPort(const std::string &portName) {
    serialPort.Open(portName);
  }

  ~exsSerialPort() override {
    if (serialPort.IsOpen()) {
      serialPort.Close();
    }
  }

  void ReadByte(char &charBuffer, unsigned long msTimeout) override {
    serialPort.ReadByte(charBuffer, msTimeout);
  }

private:
  LibSerial::SerialPort serialPort;
};

int main() {
  try {
    // Initialize the serial port and clock
    exsSerialPort SerialPort("/dev/ttyUSB0");
    Clock clock;

    // Create an instance of exf
    esx myExf(SerialPort, clock);

    // Call waitForStart to wait for NAK
    myExf.waitForStart();
    std::cout << "Successfully received NAK. Ready to proceed." << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
