#include "Credential.h"
#include "Base32.h"
#include <TOTP.h>
#include <cstring>
#include <string>

Credential::Credential() {}

Credential::Credential(String name, String secret, bool isOTP){
  this->_name = name;
  this->_secret = secret;
  this->_isOTP = isOTP;
}

String Credential::getSecret() {
  if(this->_isOTP) {
    return this->getOTP();
  }
  else {
    return this->_secret;
  }
}

String Credential::getOTP() {
  NTP ntp;
  unsigned long timestamp = ntp.getEpoch();
  long asciilength = this->_secret.length()+1;
  byte* buffer = (byte*) malloc(asciilength);
  this->_secret.getBytes(buffer, asciilength);
  asciilength -= 1;
  
  long base32length = ceil((double)asciilength/5.0)*8;
  byte* base32buffer = (byte*) malloc(base32length);
  
  Base32().fromBase32(buffer, asciilength, base32buffer);
  
  long noPaddingBase32Length = floor((double)asciilength*5.0/8);
  
  TOTP totp = TOTP(base32buffer, noPaddingBase32Length);
  return String(totp.getCode(timestamp));
}

String Credential::getName() {
  return this->_name;
}

void Credential::setName(String name) {
  this->_name = name;
}

void Credential::setSecret(String secret) {
  this->_secret = secret;
}

bool Credential::isOTP() {
  return this->_isOTP;
}

void   Credential::isOTP(bool value) {
  this->_isOTP = value;
}
