#include "exs.hpp"
#include "IClock.hpp"
#include "xmodem_const.hpp"
#include <iostream> //to be deleted
#include <stdexcept>

void exf::waitForStart() {
  char charBuffer = 0;
  auto start_time = _Clock.now();
  while (charBuffer != xmodem::NAK) {
    verifyAckTimeout(start_time);
    _Port.ReadByte(charBuffer, xmodem::WaitACK);
    std::cout << "Received char: 0x" << std::hex
              << static_cast<uint16_t>(charBuffer) << std::dec << std::endl;
  }
}

void exf::verifyAckTimeout(std::chrono::steady_clock::time_point start_time) {
  auto now = _Clock.now();
  auto elapsed_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time)
          .count();
  if (elapsed_ms > xmodem::WaitACK) {
    throw std::runtime_error("Timeout waiting for Initial NAK");
  }
  return;
}