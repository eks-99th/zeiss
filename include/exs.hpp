#pragma once
#include "IClock.hpp"
#include "Iexs.hpp"
#include <string>

class exf {
public:
  explicit exf(Iexf &port, IClock &clock) : _Port(port), _Clock(clock){};
  void waitForStart();
  ~exf() = default;

private:
  Iexf &_Port;
  IClock &_Clock;
  void verifyAckTimeout(std::chrono::steady_clock::time_point start_time);
};
