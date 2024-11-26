#include "exs.hpp"
#include "xmodem_const.hpp"

void exf::waitForStart() {
  char charBuffer = 0;
  uint8_t count = 0;
  while (charBuffer != xmodem::NAK) {
    _Port.ReadByte(charBuffer, timeout_ms);
    count++;
  }
}