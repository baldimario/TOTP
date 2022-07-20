#ifndef _CREDENTIAL_H
#define _CREDENTIAL_H

#include <Arduino.h>
#include <cstring>
#include <string>
#include "NTP.h"

typedef class Credential {
  public:
    Credential();
    Credential(String, String, bool);
    void setSecret(String);
    void setName(String);
    void isOTP(bool);
    String getSecret();
    String getName();
    bool isOTP();
  private:
    String getOTP();
    String _name;
    String _secret;
    byte* _hmacKey;
    uint8_t _length;
    bool _isOTP;
} Credential;

#endif
