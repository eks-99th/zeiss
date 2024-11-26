#include "exs.hpp"
#include "IClock.hpp"
#include "xmodem_const.hpp"
#include <iostream> //to be deleted
#include <stdexcept>

namespace {
enum class InternalStates {
  WaitInitialAck,
  SendingSoh,
  SendingBlk,
  SendingData,
  SensingCrc,
  WaitingAck,
  NumStates
};
InternalStates _state;
static uint16_t _blockNumber = 1;
} // namespace

esx::esx(Iexs &port, IClock &clock) : _Port(port), _Clock(clock) {
  _state = InternalStates::WaitInitialAck;
  _blockNumber = 1;
}

void esx::waitForStart() {
  char charBuffer = 0;
  auto start_time = _Clock.now();
  while (charBuffer != xmodem::NAK) {
    verifyAckTimeout(start_time);
    std::cout << "Waiting for NAK" << std::endl;
    _Port.ReadByte(charBuffer, xmodem::WaitACK);
    std::cout << "Received char: 0x" << std::hex
              << static_cast<uint16_t>(charBuffer) << std::dec << " ('"
              << charBuffer << "')" << std::endl;
  }
  _state = InternalStates::SendingSoh;
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
  char charBuffer = xmodem::SOH;
  _Port.WriteByte(charBuffer);
  _state = InternalStates::SendingBlk;
}

void esx::sendBlock() {
  _Port.WriteByte(_blockNumber);
  _Port.WriteByte(~_blockNumber);
  _state = InternalStates::SendingData;
}
