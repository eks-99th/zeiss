#include <cstdint>
namespace xmodem {
const uint8_t ACK = 0x06;
const uint8_t NAK = 0x15;
const uint8_t SOH = 0x01;
const uint8_t EOT = 0x04;
const uint8_t xEOF = 0x1A; // Name EOF is reserved
const uint32_t WaitACK = 5000;
const uint32_t PacketSize = 128;
} // namespace xmodem