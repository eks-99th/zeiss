#pragma once
#include "IClock.hpp"
#include "Iexs.hpp"
#include <fstream>
#include <string>

class esx {
public:
  explicit esx(Iexs &port, IClock &clock, const std::string &filePath);
  void waitForStart();
  void sendSOH();
  void sendBlock();
  void sendDataCrc();
  void waitForAck();
  void sendEOT();
  ~esx();

private:
  Iexs &_Port;
  IClock &_Clock;
  const std::string &_filePath;
  std::ifstream _fileStream;
  void verifyAckTimeout(std::chrono::steady_clock::time_point start_time);
};
