#include "WiFiRecord.h"

WiFiRecord::WiFiRecord(String ssid, String password) {
  this->_ssid = ssid;
  this->_password = password;
}

String WiFiRecord::getSSID() {
  return this->_ssid;
}

String WiFiRecord::getPassword() {
  return this->_password;
}
