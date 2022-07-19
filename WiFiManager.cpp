#include "WiFiManager.h"
#include <WiFi.h>
#include "WiFiRecord.h"

WiFiManager::WiFiManager(WiFiRecord* credentials) {
  this->_credentials = credentials;
}

WiFiRecord WiFiManager::getCredentials(int index) {
  return this->_credentials[index];
}

WiFiRecord WiFiManager::getCredentials(String ssid) {
  long array_size = sizeof(this->_credentials)/sizeof(this->_credentials[0]);
  for(int i = 0; i < array_size; i++) {
    if(this->_credentials[i].getSSID() == ssid) {
      return this->_credentials[i];
    }
  }
}

void WiFiManager::connect(WiFiCredentials credentials) {
  WiFi.begin(credentials.getSSID().c_str(), credentials.getPassword().c_str());
}

void WiFiManager::connect(int index) {
  this->connect(this->getCredentials(index));
}

void WiFiManager::connect(String ssid) {
  this->connect(this->getCredentials(ssid));
}

bool WiFiManager::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}
