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

const std::string &dummyFilePath = "dummy.txt";

class MockSerialPort : public Iexs {
public:
  MOCK_METHOD(void, ReadByte, (char &charBuffer, unsigned long msTimeout),
              (override));
  MOCK_METHOD(void, WriteByte, (char charBuffer), (override));
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

  auto testobjEXF = new esx(mockSerial, mockClock, dummyFilePath);
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

  auto testobjEXF = new esx(mockSerial, mockClock, dummyFilePath);
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

  auto testobjEXF = new esx(mockSerial, mockClock, dummyFilePath);
  EXPECT_THROW(testobjEXF->waitForStart(), std::runtime_error);
}

TEST(StartUp1, AfterNakSendsSOH) {
  // The user calls the waitForStart() method and the xmodem::NAK is received
  //  in the first call to ReadByte, after that SOH is sent
  MockSerialPort mockSerial;
  MockClock mockClock;

  // Set up the time interface
  auto start_time = std::chrono::steady_clock::now();
  EXPECT_CALL(mockClock, now()).WillRepeatedly(Return(start_time));

  // Set up the serial
  EXPECT_CALL(mockSerial, ReadByte(_, xmodem::WaitACK))
      .WillOnce(DoAll(SetArgReferee<0>(xmodem::NAK), Return()));
  EXPECT_CALL(mockSerial, WriteByte(xmodem::SOH)).Times(1);

  auto testobjEXF = new esx(mockSerial, mockClock, dummyFilePath);
  testobjEXF->waitForStart();
  testobjEXF->sendSOH();
}

TEST(StartUp1, AfterNakAndSOHSendsBLK) {
  // The user calls the waitForStart() method and the xmodem::NAK is received.
  //  After that, the SOH is sent, followed by the BLK and ~BLK.
  MockSerialPort mockSerial;
  MockClock mockClock;

  // Set up the time interface
  auto start_time = std::chrono::steady_clock::now();
  EXPECT_CALL(mockClock, now()).WillRepeatedly(Return(start_time));

  // Set up the serial
  EXPECT_CALL(mockSerial, ReadByte(_, xmodem::WaitACK))
      .WillOnce(DoAll(SetArgReferee<0>(xmodem::NAK), Return()));
  {
    ::testing::InSequence seq;

    EXPECT_CALL(mockSerial, WriteByte(xmodem::SOH)).Times(1); // Expect SOH
    EXPECT_CALL(mockSerial, WriteByte(0x01)).Times(1);        // Expect BLK
    EXPECT_CALL(mockSerial, WriteByte(0xFE)).Times(1);        // Expect ~BLK
  }

  auto testobjEXF = new esx(mockSerial, mockClock, dummyFilePath);
  testobjEXF->waitForStart();
  testobjEXF->sendSOH();
  testobjEXF->sendBlock();
}

void expectWriteBytes(MockSerialPort &mockSerial,
                      const std::vector<char> &data) {
  for (const auto &byte : data) {
    EXPECT_CALL(mockSerial, WriteByte(byte)).Times(1);
  }
}

TEST(SendData, SendSmallFile) {
  // Setup
  MockSerialPort mockSerial;
  MockClock mockClock;

  // Define the test file details
  const std::string filePath = "./small.txt";
  const std::vector<char> fileData = {'H', 'e', 'l', 'l', 'o', ' ',
                                      'f', 'r', 'o', 'm', ' ', 'S',
                                      'e', 'n', 'd', '.', '\n'};
  const size_t paddingSize = xmodem::PacketSize - fileData.size();
  const char fileCRC = 0xF0; // Example CRC value (should match the test setup)
  const char BLK = 0x01;     // Block number
  const char nBLK = 0xFE;    // Complement of block number

  // Mock the clock behavior
  auto start_time = std::chrono::steady_clock::now();
  EXPECT_CALL(mockClock, now()).WillRepeatedly(Return(start_time));

  // Mock the serial behavior for ReadByte
  EXPECT_CALL(mockSerial, ReadByte(_, xmodem::WaitACK))
      .WillOnce(DoAll(SetArgReferee<0>(xmodem::NAK), Return()))
      .WillOnce(DoAll(SetArgReferee<0>(xmodem::ACK), Return()))
      .WillOnce(DoAll(SetArgReferee<0>(xmodem::ACK), Return()));

  // Set up the serial expectations
  {
    ::testing::InSequence seq;

    // Control bytes
    EXPECT_CALL(mockSerial, WriteByte(xmodem::SOH)).Times(1); // Start of Header
    EXPECT_CALL(mockSerial, WriteByte(BLK)).Times(1);         // Block number
    EXPECT_CALL(mockSerial, WriteByte(nBLK))
        .Times(1); // Complement of block number

    // File data
    expectWriteBytes(mockSerial, fileData);

    // Padding for the remaining bytes
    EXPECT_CALL(mockSerial, WriteByte(xmodem::xEOF)).Times(paddingSize);

    // CRC byte
    EXPECT_CALL(mockSerial, WriteByte(fileCRC)).Times(1);
    // EOT byte
    EXPECT_CALL(mockSerial, WriteByte(xmodem::EOT)).Times(1);
  }

  // Create the test object
  auto testobjEXF = std::make_unique<esx>(mockSerial, mockClock, filePath);

  // Execution steps
  testobjEXF->waitForStart(); // Wait for initial NAK
  testobjEXF->sendSOH();      // Send SOH
  testobjEXF->sendBlock();    // Send block number and its complement
  testobjEXF->sendDataCrc();  // Send data and CRC
  testobjEXF->waitForAck();   // Wait for ACK
  testobjEXF->sendEOT();      // Send SOH
  testobjEXF->waitForAck();   // Wait for ACK
}