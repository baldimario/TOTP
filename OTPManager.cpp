#include "OTPManager.h"
#include "OTP.h"

OTPManager::OTPManager(OTP* _totps) {
  this->_totps = _totps;
}

OTP OTPManager::getOTP(int index) {
  return this->_totps[index];
}

OTP OTPManager::getOTP(String name) {
  long array_size = sizeof(this->_totps)/sizeof(this->_totps[0]);
  for(int i = 0; i < array_size; i++) {
    if(this->_totps[i].getName() == name) {
      return this->_totps[i];
    }
  }
}
