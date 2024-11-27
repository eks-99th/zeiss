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

// TEST(StartUp1, WaitForStartReceiveNAK) {
//   // The user calls the waitForStart() method and the xmodem::NAK is received
//   //  in the first call to ReadByte
//   MockSerialPort mockSerial;
//   MockClock mockClock;

//   // Set up the time interface
//   auto start_time = std::chrono::steady_clock::now();
//   EXPECT_CALL(mockClock, now()).WillRepeatedly(Return(start_time));

//   // Set up the serial
//   EXPECT_CALL(mockSerial, ReadByte(_, xmodem::WaitACK))
//       .WillOnce(DoAll(SetArgReferee<0>(xmodem::NAK), Return()));

//   auto testobjEXF = new esx(mockSerial, mockClock, dummyFilePath);
//   testobjEXF->waitForStart();
// }

// TEST(StartUp, WaitForStartReceiveNAKOn3RD) {
//   // The user calls the waitForStart() method and the xmodem::NAK is received
//   //  in the 3rd call to ReadByte, each byte is received 500ms apart
//   MockSerialPort mockSerial;
//   MockClock mockClock;

//   // Set up the expectation for ReadByte
//   EXPECT_CALL(mockSerial, ReadByte(_, xmodem::WaitACK))
//       .WillOnce(DoAll(SetArgReferee<0>('A'), Return()))
//       .WillOnce(DoAll(SetArgReferee<0>('B'), Return()))
//       .WillOnce(DoAll(SetArgReferee<0>(xmodem::NAK), Return()));

//   auto start_time = std::chrono::steady_clock::now();
//   auto after_500ms = start_time + std::chrono::milliseconds(500);
//   auto after_1500ms = start_time + std::chrono::milliseconds(1500);
//   auto after_2000ms = start_time + std::chrono::milliseconds(2000);

//   EXPECT_CALL(mockClock, now())
//       .WillOnce(Return(start_time))    // First call to now()
//       .WillOnce(Return(after_500ms))   // First byte received
//       .WillOnce(Return(after_1500ms))  // Second byte received
//       .WillOnce(Return(after_2000ms)); // NAK received

//   auto testobjEXF = new esx(mockSerial, mockClock, dummyFilePath);
//   testobjEXF->waitForStart();
// }

// TEST(StartUp, WaitForStartReceiveTimesOut) {
//   // The user calls the waitForStart() method and the xmodem::NAK is received
//   //  in the 3rd call to ReadByte, each byte is received 500ms apart
//   MockSerialPort mockSerial;
//   MockClock mockClock;

//   // Set up the expectation for ReadByte
//   EXPECT_CALL(mockSerial, ReadByte(_, xmodem::WaitACK))
//       .WillRepeatedly(DoAll(SetArgReferee<0>('A'), Return()));

//   auto start_time = std::chrono::steady_clock::now();
//   auto timeout_ms = start_time + std::chrono::milliseconds(xmodem::WaitACK +
//   1);

//   EXPECT_CALL(mockClock, now())
//       .WillOnce(Return(start_time))  // First call to now()
//       .WillOnce(Return(timeout_ms)); // First byte received

//   auto testobjEXF = new esx(mockSerial, mockClock, dummyFilePath);
//   EXPECT_THROW(testobjEXF->waitForStart(), std::runtime_error);
// }

// TEST(StartUp1, AfterNakSendsSOH) {
//   // The user calls the waitForStart() method and the xmodem::NAK is received
//   //  in the first call to ReadByte, after that SOH is sent
//   MockSerialPort mockSerial;
//   MockClock mockClock;

//   // Set up the time interface
//   auto start_time = std::chrono::steady_clock::now();
//   EXPECT_CALL(mockClock, now()).WillRepeatedly(Return(start_time));

//   // Set up the serial
//   EXPECT_CALL(mockSerial, ReadByte(_, xmodem::WaitACK))
//       .WillOnce(DoAll(SetArgReferee<0>(xmodem::NAK), Return()));
//   EXPECT_CALL(mockSerial, WriteByte(xmodem::SOH)).Times(1);

//   auto testobjEXF = new esx(mockSerial, mockClock, dummyFilePath);
//   testobjEXF->waitForStart();
//   testobjEXF->sendSOH();
// }

// TEST(StartUp1, AfterNakAndSOHSendsBLK) {
//   // The user calls the waitForStart() method and the xmodem::NAK is
//   received.
//   //  After that, the SOH is sent, followed by the BLK and ~BLK.
//   MockSerialPort mockSerial;
//   MockClock mockClock;

//   // Set up the time interface
//   auto start_time = std::chrono::steady_clock::now();
//   EXPECT_CALL(mockClock, now()).WillRepeatedly(Return(start_time));

//   // Set up the serial
//   EXPECT_CALL(mockSerial, ReadByte(_, xmodem::WaitACK))
//       .WillOnce(DoAll(SetArgReferee<0>(xmodem::NAK), Return()));
//   {
//     ::testing::InSequence seq;

//     EXPECT_CALL(mockSerial, WriteByte(xmodem::SOH)).Times(1); // Expect SOH
//     EXPECT_CALL(mockSerial, WriteByte(0x01)).Times(1);        // Expect BLK
//     EXPECT_CALL(mockSerial, WriteByte(0xFE)).Times(1);        // Expect ~BLK
//   }

//   auto testobjEXF = new esx(mockSerial, mockClock, dummyFilePath);
//   testobjEXF->waitForStart();
//   testobjEXF->sendSOH();
//   testobjEXF->sendBlock();
// }

void expectBlock(MockSerialPort &mockSerial, const std::vector<char> &block,
                 char blk, char nblk, const size_t paddingSize,
                 const char blockCRC) {
  // Control bytes
  EXPECT_CALL(mockSerial, WriteByte(xmodem::SOH)).Times(1); // Start of
  EXPECT_CALL(mockSerial, WriteByte(blk)).Times(1);         // Block
  EXPECT_CALL(mockSerial, WriteByte(nblk)).Times(1);        // negative block

  // Block Data
  for (const auto &byte : block) {
    EXPECT_CALL(mockSerial, WriteByte(byte)).Times(1);
  }
  // Verify padding
  if (paddingSize > 0) {
    EXPECT_CALL(mockSerial, WriteByte(xmodem::xEOF)).Times(paddingSize);
  }
  EXPECT_CALL(mockSerial, WriteByte(blockCRC)).Times(1); // Block CRC
  EXPECT_CALL(mockSerial, ReadByte(_, xmodem::WaitACK))
      .WillOnce(DoAll(SetArgReferee<0>(xmodem::ACK), Return()));
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
  const char blockCRC = 0xF0; // Example CRC value
  const char BLK = 0x01;      // Block number
  const char nBLK = 0xFE;     // n block number

  // Mock the clock behavior
  auto start_time = std::chrono::steady_clock::now();
  EXPECT_CALL(mockClock, now()).WillRepeatedly(Return(start_time));
  EXPECT_CALL(mockSerial, ReadByte(_, xmodem::WaitACK))
      .WillOnce(DoAll(SetArgReferee<0>(xmodem::NAK), Return()));

  // Set up the expectations for first block
  {
    ::testing::InSequence seq;
    expectBlock(mockSerial, fileData, BLK, nBLK, paddingSize, blockCRC);
    EXPECT_CALL(mockSerial, WriteByte(xmodem::EOT)).Times(1);
    EXPECT_CALL(mockSerial, ReadByte(_, xmodem::WaitACK))
        .WillOnce(DoAll(SetArgReferee<0>(xmodem::ACK), Return()));
  }

  // Create the test object
  auto testobjEXF = std::make_unique<esx>(mockSerial, mockClock, filePath);
  testobjEXF->send();
}

TEST(SendData, SendMediumFile) {
  // Setup
  MockSerialPort mockSerial;
  MockClock mockClock;

  // Define the test file details
  const std::string mediumFilePath = "./medium.txt";
  const std::vector<char> firstBlock = {
      'L', 'o',  'r', 'e', 'm', ' ', 'i', 'p', 's', 'u', 'm', ' ', 'd',
      'o', 'l',  'o', 'r', ' ', 's', 'i', 't', ' ', 'a', 'm', 'e', 't',
      ',', ' ',  'c', 'o', 'n', 's', 'e', 'c', 't', 'e', 't', 'u', 'r',
      ' ', 'a',  'd', 'i', 'p', 'i', 's', 'c', 'i', 'n', 'g', ' ', 'e',
      'l', 'i',  't', '.', ' ', 'S', 'e', 'd', ' ', 'd', 'o', ' ', 'e',
      'i', 'u',  's', 'm', 'o', 'd', ' ', 't', 'e', 'm', 'p', 'o', 'r',
      ' ', '\n', 'i', 'n', 'c', 'i', 'd', 'i', 'd', 'u', 'n', 't', ' ',
      'u', 't',  ' ', 'l', 'a', 'b', 'o', 'r', 'e', ' ', 'e', 't', ' ',
      'd', 'o',  'l', 'o', 'r', 'e', ' ', 'm', 'a', 'g', 'n', 'a', ' ',
      'a', 'l',  'i', 'q', 'u', 'a', '.', ' ', 'U', 't', ' '};
  const char firstBlockCRC = 0x7B;
  const std::vector<char> secondBlock = {
      'e', 'n',  'i',  'm', ' ', 'a', 'd', ' ', 'm', 'i', 'n', 'i', 'm',
      ' ', 'v',  'e',  'n', 'i', 'a', 'm', ',', ' ', 'q', 'u', 'i', 's',
      ' ', '\n', 'n',  'o', 's', 't', 'r', 'u', 'd', ' ', 'e', 'x', 'e',
      'r', 'c',  'i',  't', 'a', 't', 'i', 'o', 'n', ' ', 'u', 'l', 'l',
      'a', 'm',  'c',  'o', ' ', 'l', 'a', 'b', 'o', 'r', 'i', 's', ' ',
      'n', 'i',  's',  'i', ' ', 'u', 't', ' ', 'a', 'l', 'i', 'q', 'u',
      'i', 'p',  ' ',  'e', 'x', ' ', 'e', 'a', ' ', 'c', 'o', 'm', 'm',
      'o', 'd',  'o',  ' ', 'c', 'o', 'n', 's', 'e', 'q', 'u', 'a', 't',
      '.', ' ',  '\n', 'D', 'u', 'i', 's', ' ', 'a', 'u', 't', 'e', ' ',
      'i', 'r',  'u',  'r', 'e', ' ', 'd', 'o', 'l', 'o', 'r'};
  const char secondBlockCRC = 0x0F;
  const std::vector<char> thirdBlock = {
      ' ', 'i', 'n', ' ', 'r', 'e', 'p', 'r',  'e', 'h', 'e', 'n', 'd',
      'e', 'r', 'i', 't', ' ', 'i', 'n', ' ',  'v', 'o', 'l', 'u', 'p',
      't', 'a', 't', 'e', ' ', 'v', 'e', 'l',  'i', 't', ' ', 'e', 's',
      's', 'e', ' ', 'c', 'i', 'l', 'l', 'u',  'm', ' ', 'd', 'o', 'l',
      'o', 'r', 'e', ' ', 'e', 'u', ' ', '\n', 'f', 'u', 'g', 'i', 'a',
      't', ' ', 'n', 'u', 'l', 'l', 'a', ' ',  'p', 'a', 'r', 'i', 'a',
      't', 'u', 'r', '.', ' ', 'E', 'x', 'c',  'e', 'p', 't', 'e', 'u',
      'r', ' ', 's', 'i', 'n', 't', ' ', 'o',  'c', 'c', 'a', 'e', 'c',
      'a', 't', ' ', 'c', 'u', 'p', 'i', 'd',  'a', 't', 'a', 't', ' ',
      'n', 'o', 'n', ' ', 'p', 'r', 'o', 'i',  'd', 'e', 'n'};
  const char thirdBlockCRC = 0xD9;
  const std::vector<char> fourthBlock = {
      't', ',', ' ', 's', 'u', 'n', 't', ' ',  'i', 'n', ' ', '\n', 'c',
      'u', 'l', 'p', 'a', ' ', 'q', 'u', 'i',  ' ', 'o', 'f', 'f',  'i',
      'c', 'i', 'a', ' ', 'd', 'e', 's', 'e',  'r', 'u', 'n', 't',  ' ',
      'm', 'o', 'l', 'l', 'i', 't', ' ', 'a',  'n', 'i', 'm', ' ',  'i',
      'd', ' ', 'e', 's', 't', ' ', 'l', 'a',  'b', 'o', 'r', 'u',  'm',
      '.', ' ', 'C', 'u', 'r', 'a', 'b', 'i',  't', 'u', 'r', ' ',  'p',
      'r', 'e', 't', 'i', 'u', 'm', ' ', '\n', 't', 'i', 'n', 'c',  'i',
      'd', 'u', 'n', 't', ' ', 'l', 'a', 'c',  'u', 's', '.', ' ',  'N',
      'u', 'l', 'l', 'a', ' ', 'g', 'r', 'a',  'v', 'i', 'd', 'a',  ' ',
      'o', 'r', 'c', 'i', ' ', 'a', ' ', 'o',  'd', 'i', 'o'};
  const char fourthBlockCRC = 0x45;
  const std::vector<char> fifthBlock = {
      '.', ' ', 'N', 'u', 'l', 'l', 'a', 'm', ' ', 'v', 'a', 'r',
      'i', 'u', 's', ',', ' ', 't', 'u', 'r', 'p', 'i', 's', ' ',
      'e', 't', ' ', 'c', 'o', 'm', 'm', 'o', 'd', 'o', ' ', '\n',
      'p', 'h', 'a', 'r', 'e', 't', 'r', 'a', '.', '\n'};
  const char fifthBlockCRC = 0x78;
  const size_t fifthBlockpadding = xmodem::PacketSize - fifthBlock.size();
  const size_t noBlockpadding = 0;

  // Mock the clock behavior
  auto start_time = std::chrono::steady_clock::now();
  EXPECT_CALL(mockClock, now()).WillRepeatedly(Return(start_time));

  // Mock the serial behavior for ReadByte
  EXPECT_CALL(mockSerial, ReadByte(_, xmodem::WaitACK))
      .WillOnce(DoAll(SetArgReferee<0>(xmodem::NAK), Return()));

  // Set up the expectations for first block
  {
    ::testing::InSequence seq;
    expectBlock(mockSerial, firstBlock, 0x01, 0xFE, noBlockpadding,
                firstBlockCRC);
    expectBlock(mockSerial, secondBlock, 0x02, 0xFD, noBlockpadding,
                secondBlockCRC);
    expectBlock(mockSerial, thirdBlock, 0x03, 0xFC, noBlockpadding,
                thirdBlockCRC);
    expectBlock(mockSerial, fourthBlock, 0x04, 0xFB, noBlockpadding,
                fourthBlockCRC);
    expectBlock(mockSerial, fifthBlock, 0x05, 0xFA, fifthBlockpadding,
                fifthBlockCRC);
    EXPECT_CALL(mockSerial, WriteByte(xmodem::EOT)).Times(1);
    EXPECT_CALL(mockSerial, ReadByte(_, xmodem::WaitACK))
        .WillOnce(DoAll(SetArgReferee<0>(xmodem::ACK), Return()));
  }

  // Create the test object
  auto testobjEXF =
      std::make_unique<esx>(mockSerial, mockClock, mediumFilePath);
  testobjEXF->send();
}