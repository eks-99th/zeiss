#include "IClock.hpp"
#include "argparse.hpp"
#include "exs.hpp"
#include "xmodem_const.hpp"
#include <chrono>
#include <fstream>
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
    serialPort.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
    serialPort.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
    serialPort.SetStopBits(LibSerial::StopBits::STOP_BITS_1);
    serialPort.SetParity(LibSerial::Parity::PARITY_NONE);
  }

  ~exsSerialPort() override {
    if (serialPort.IsOpen()) {
      serialPort.Close();
    }
  }

  void ReadByte(char &charBuffer, unsigned long msTimeout) override {
    serialPort.ReadByte(charBuffer, msTimeout);
  }

  void WriteByte(char charBuffer) override { serialPort.WriteByte(charBuffer); }

private:
  LibSerial::SerialPort serialPort;
};

int main(int argc, char *argv[]) {
  // Argument parsing with argparse
  argparse::ArgumentParser program("xmodem_transfer");

  program.add_argument("-f", "--file")
      .required()
      .help("Path to the file to be transferred");

  program.add_argument("-p", "--port")
      .required()
      .help("Serial port to use for communication (e.g., /dev/ttyUSB0)");

  program.add_argument("-h", "--help")
      .default_value(false)
      .implicit_value(true)
      .help("Show this help message and exit");

  try {
    program.parse_args(argc, argv);

    if (program["--help"] == true) {
      std::cout << program;
      return 0;
    }

    std::string filePath = program.get<std::string>("--file");
    std::string serialPortName = program.get<std::string>("--port");

    // Initialize the serial port and clock
    exsSerialPort serialPort(serialPortName);
    Clock clock;

    // Create an instance of esx
    esx myExf(serialPort, clock, filePath);
    myExf.waitForStart(); // Wait for initial NAK
    while (!myExf.FileTransferFinished()) {
      myExf.sendSOH();     // Send SOH
      myExf.sendBlock();   // Send block number and its complement
      myExf.sendDataCrc(); // Send data and CRC
      myExf.waitForAck();  // Wait for ACK
    }
    myExf.sendEOT();    // Send EOT
    myExf.waitForAck(); // Wait for ACK

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}