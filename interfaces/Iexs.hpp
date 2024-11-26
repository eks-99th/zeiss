#pragma once

class Iexf {
public:
  virtual ~Iexf() = default;

  // Pure virtual method for reading a byte
  virtual void ReadByte(char &charBuffer, unsigned long msTimeout = 0) = 0;
};
