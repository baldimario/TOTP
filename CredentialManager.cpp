#include "CredentialManager.h"
#include "Credential.h"

CredentialManager::CredentialManager() {};

CredentialManager::CredentialManager(Credential* _credentials, unsigned long size) {
  this->_credentials = _credentials;
  this->_size = size;
}

void CredentialManager::setCredentials(Credential* _credentials, unsigned long size) {
  this->_credentials = _credentials;
  this->_size = size;
}

Credential CredentialManager::getCredential(int index) {
  return this->_credentials[index];
}

Credential CredentialManager::getCredential(String name) {
  long array_size = sizeof(this->_credentials)/sizeof(*this->_credentials);
  for(int i = 0; i < array_size; i++) {
    if(this->_credentials[i].getName() == name) {
      return this->_credentials[i];
    }
  }
}

unsigned long CredentialManager::getSize() {
  return this->_size;
}
