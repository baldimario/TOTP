#include "OTP.h"
#include "Base32.h"
#include <TOTP.h>
#include <cstring>
#include <string>

OTP::OTP(String name, String secret){
  this->_name = name;
  this->_secret = secret;
}

OTP::OTP(String name, byte* hmacKey, uint8_t length){
  this->_name = name;
  this->_hmacKey = hmacKey;
  this->_length = length;
}


String OTP::getToken(unsigned long timestamp) {
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

/*String OTP::getToken(unsigned long timestamp) {
  long asciilength = this->_secret.length()+1;
  byte buffer[100]; //(byte*) malloc(asciilength);
  this->_secret.getBytes(buffer, asciilength);
  asciilength -= 1;
  
  long base32length = ceil((double)asciilength/5.0)*8;
  byte base32buffer[100]; // = (byte*) malloc(base32length);

  byte* bufferpointer = &buffer[0];
  byte* base32bufferpointer = &base32buffer[0];
  Base32().fromBase32(bufferpointer, asciilength, base32bufferpointer);
  
  long noPaddingBase32Length = floor((double)asciilength*5.0/8);
  
  TOTP totp = TOTP(base32buffer, noPaddingBase32Length);
  return String(totp.getCode(timestamp));
}*/


String OTP::getName() {
  return this->_name;
}
