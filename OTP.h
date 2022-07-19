#ifndef _OTP_H
#define _OTP_H

#include <Arduino.h>
#include <cstring>
#include <string>

typedef class OTP {
  public:
    OTP(String, String);
    OTP(String, byte*, uint8_t length);
    String getToken(unsigned long);
    String getName();
  private:
    String _name;
    String _secret;
    byte* _hmacKey;
    uint8_t _length;
} OTP;

#endif
