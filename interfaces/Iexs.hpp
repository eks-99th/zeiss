#pragma once

class Iexs {
public:
  virtual ~Iexs() = default;

  // Pure virtual method for reading a byte
  virtual void ReadByte(char &charBuffer, unsigned long msTimeout = 0) = 0;
};
