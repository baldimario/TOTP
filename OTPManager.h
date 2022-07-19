#ifndef _OTPMANAGER_H
#define _OTPMANAGER_H

#include "OTP.h"

typedef class OTPManager {
  public:
    OTPManager(OTP*);
    OTP getOTP(int);
    OTP getOTP(String);
   private:
    OTP* _totps;
} OTPManager;

#endif
