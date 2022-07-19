#ifndef Base32_h
#define Base32_h

#include "Arduino.h"
#include "stdint.h"

class Base32
{
  public:
    Base32();
    int toBase322(const char*, long, byte*&);
    int toBase32(byte*, long, byte*&, boolean);
    int fromBase32(byte*, long, byte*&);
};

#endif
