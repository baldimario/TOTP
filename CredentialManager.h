#ifndef _CredentialMANAGER_H
#define _CredentialMANAGER_H

#include "Credential.h"

typedef class CredentialManager {
  public:
    CredentialManager();
    CredentialManager(Credential*, unsigned long);
    void setCredentials(Credential*, unsigned long);
    Credential getCredential(int);
    Credential getCredential(String);
    unsigned long getSize();
   private:
    Credential* _credentials;
    unsigned long _size = 0;
} CredentialManager;

#endif
