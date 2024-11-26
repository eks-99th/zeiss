#include "Iexs.hpp"
#include "exs.hpp"
#include "xmodem_const.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgReferee;

class MockSerialPort : public Iexf {
public:
  MOCK_METHOD(void, ReadByte, (char &charBuffer, unsigned long msTimeout),
              (override));
};

TEST(StartUp, WaitForStartReceiveNAK) {
  MockSerialPort mockSerial;
  const u_int32_t timeout_ms = 1000;

  // Set up the expectation for ReadByte
  EXPECT_CALL(mockSerial, ReadByte(_, timeout_ms))
      .WillOnce(DoAll(SetArgReferee<0>(xmodem::NAK), Return()));

  auto testobjEXF = new exf(mockSerial, timeout_ms);
  testobjEXF->waitForStart();
}

TEST(StartUp, WaitForStartReceiveNAKOn3RD) {
  MockSerialPort mockSerial;
  const u_int32_t timeout_ms = 1000;

  // Set up the expectation for ReadByte
  EXPECT_CALL(mockSerial, ReadByte(_, timeout_ms))
      .WillOnce(DoAll(SetArgReferee<0>('A'), Return()))
      .WillOnce(DoAll(SetArgReferee<0>('B'), Return()))
      .WillOnce(DoAll(SetArgReferee<0>(xmodem::NAK), Return()));

  auto testobjEXF = new exf(mockSerial, timeout_ms);
  testobjEXF->waitForStart();
}