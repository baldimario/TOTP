#ifndef _WIFIRECORD_H
#define _WIFIRECORD_H

#include <Arduino.h>

typedef class WiFiRecord {
  public:
    WiFiRecord(String, String);
    String getSSID();
    String getPassword();
  private:
    String _ssid;
    String _password;
} WiFiCredentials;

#endif
