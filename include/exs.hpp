#pragma once
#include "IClock.hpp"
#include "Iexs.hpp"
#include <string>

class esx {
public:
  explicit esx(Iexs &port, IClock &clock);
  void waitForStart();
  void sendSOH();
  void sendBlock();
  ~esx() = default;

private:
  Iexs &_Port;
  IClock &_Clock;
  void verifyAckTimeout(std::chrono::steady_clock::time_point start_time);
};
