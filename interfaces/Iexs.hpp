#pragma once

class Iexs {
public:
  virtual ~Iexs() = default;

  virtual void ReadByte(char &charBuffer, unsigned long msTimeout) = 0;
  virtual void WriteByte(char charBuffer) = 0;
};
