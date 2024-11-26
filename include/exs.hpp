#pragma once
#include "Iexs.hpp"
#include <string>

class exf {
public:
  explicit exf(Iexf &port, uint32_t timeout_ms)
      : _Port(port), timeout_ms(timeout_ms){};
  void waitForStart();
  ~exf() = default;

private:
  Iexf &_Port;
  uint32_t timeout_ms;
};
