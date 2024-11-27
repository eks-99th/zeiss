#include "exs.hpp"
#include "IClock.hpp"
#include "xmodem_const.hpp"
#include <iomanip>
#include <iostream> //to be deleted
#include <stdexcept>

namespace {
enum class InternalStates {
  WaitInitialAck,
  SendingSoh,
  SendingBlk,
  SendingData,
  SendingCrc,
  WaitingAck,
  NumStates
};
InternalStates _state;
static uint16_t _blockNumber = 1;
} // namespace

void verifyValidFileStream(const std::ifstream &fileStream,
                           const std::string &filePath) {
  if (!fileStream.is_open()) {
    throw std::runtime_error("Failed to open file: " + filePath);
  }
}

esx::esx(Iexs &port, IClock &clock, const std::string &filePath)
    : _Port(port), _Clock(clock), _filePath(filePath),
      _fileStream(filePath, std::ios::binary) {
  verifyValidFileStream(_fileStream, _filePath);
  _state = InternalStates::WaitInitialAck;
  _blockNumber = 1;
}

esx::~esx() {
  if (_fileStream.is_open()) {
    _fileStream.close();
  }
}

void esx::waitForStart() {
  char charBuffer = 0;
  auto start_time = _Clock.now();
  std::cout << "Waiting for start..." << std::endl;
  while (charBuffer != xmodem::NAK) {
    verifyAckTimeout(start_time);
    _Port.ReadByte(charBuffer, xmodem::WaitACK);
  }
  std::cout << "Start received." << std::endl;
  _state = InternalStates::SendingSoh;
}

void esx::waitForAck() {
  std::cout << "Waiting for ACK..." << std::endl;
  char charBuffer = 0;
  auto start_time = _Clock.now();
  while (charBuffer != xmodem::ACK) {
    verifyAckTimeout(start_time);
    _Port.ReadByte(charBuffer, xmodem::WaitACK);
  }
  std::cout << "ACK received." << std::endl;
}

void esx::verifyAckTimeout(std::chrono::steady_clock::time_point start_time) {
  auto now = _Clock.now();
  auto elapsed_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time)
          .count();
  if (elapsed_ms > xmodem::WaitACK) {
    throw std::runtime_error("Timeout waiting for Initial NAK");
  }
  return;
}

void esx::sendSOH() {
  std::cout << "Sending SOH..." << std::endl;
  _Port.WriteByte(xmodem::SOH);
  _state = InternalStates::SendingBlk;
  std::cout << "SOH sent." << std::endl;
}

void esx::sendEOT() {
  std::cout << "Sending EOT..." << std::endl;
  _Port.WriteByte(xmodem::EOT);
  _state = InternalStates::WaitingAck;
  std::cout << "EOT sent." << std::endl;
}

void esx::sendBlock() {
  std::cout << "Sending BLOCK..." << std::endl;
  _Port.WriteByte(_blockNumber);
  _Port.WriteByte(~_blockNumber);
  _state = InternalStates::SendingData;
  std::cout << "BLK sent." << std::endl;
}

void esx::sendDataCrc() {
  // Create a buffer to hold the data filled with xmodem::xEOF
  std::cout << "Sending DATA & CRC..." << std::endl;
  char dataBuffer[xmodem::PacketSize];
  char crc = 0;
  std::fill(std::begin(dataBuffer), std::end(dataBuffer), xmodem::xEOF);
  verifyValidFileStream(_fileStream, _filePath);
  _fileStream.read(dataBuffer, xmodem::PacketSize);
  for (auto b : dataBuffer) {
    _Port.WriteByte(b);
    crc += b;
  }
  _Port.WriteByte(crc);
  _state = InternalStates::SendingCrc;
  std::cout << "DATA & CRC sent." << std::endl;
}
