#include "IClock.hpp"
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

class MockClock : public IClock {
public:
  MOCK_METHOD(std::chrono::steady_clock::time_point, now, (),
              (const, override));
};

TEST(StartUp1, WaitForStartReceiveNAK) {
  // The user calls the waitForStart() method and the xmodem::NAK is received
  //  in the first call to ReadByte
  MockSerialPort mockSerial;
  MockClock mockClock;

  // Set up the time interface
  auto start_time = std::chrono::steady_clock::now();
  EXPECT_CALL(mockClock, now()).WillRepeatedly(Return(start_time));

  // Set up the serial
  EXPECT_CALL(mockSerial, ReadByte(_, xmodem::WaitACK))
      .WillOnce(DoAll(SetArgReferee<0>(xmodem::NAK), Return()));

  auto testobjEXF = new exf(mockSerial, mockClock);
  testobjEXF->waitForStart();
}

TEST(StartUp, WaitForStartReceiveNAKOn3RD) {
  // The user calls the waitForStart() method and the xmodem::NAK is received
  //  in the 3rd call to ReadByte, each byte is received 500ms apart
  MockSerialPort mockSerial;
  MockClock mockClock;

  // Set up the expectation for ReadByte
  EXPECT_CALL(mockSerial, ReadByte(_, xmodem::WaitACK))
      .WillOnce(DoAll(SetArgReferee<0>('A'), Return()))
      .WillOnce(DoAll(SetArgReferee<0>('B'), Return()))
      .WillOnce(DoAll(SetArgReferee<0>(xmodem::NAK), Return()));

  auto start_time = std::chrono::steady_clock::now();
  auto after_500ms = start_time + std::chrono::milliseconds(500);
  auto after_1500ms = start_time + std::chrono::milliseconds(1500);
  auto after_2000ms = start_time + std::chrono::milliseconds(2000);

  EXPECT_CALL(mockClock, now())
      .WillOnce(Return(start_time))    // First call to now()
      .WillOnce(Return(after_500ms))   // First byte received
      .WillOnce(Return(after_1500ms))  // Second byte received
      .WillOnce(Return(after_2000ms)); // NAK received

  auto testobjEXF = new exf(mockSerial, mockClock);
  testobjEXF->waitForStart();
}

TEST(StartUp, WaitForStartReceiveTimesOut) {
  // The user calls the waitForStart() method and the xmodem::NAK is received
  //  in the 3rd call to ReadByte, each byte is received 500ms apart
  MockSerialPort mockSerial;
  MockClock mockClock;

  // Set up the expectation for ReadByte
  EXPECT_CALL(mockSerial, ReadByte(_, xmodem::WaitACK))
      .WillRepeatedly(DoAll(SetArgReferee<0>('A'), Return()));

  auto start_time = std::chrono::steady_clock::now();
  auto timeout_ms = start_time + std::chrono::milliseconds(xmodem::WaitACK + 1);

  EXPECT_CALL(mockClock, now())
      .WillOnce(Return(start_time))  // First call to now()
      .WillOnce(Return(timeout_ms)); // First byte received

  auto testobjEXF = new exf(mockSerial, mockClock);
  EXPECT_THROW(testobjEXF->waitForStart(), std::runtime_error);
}