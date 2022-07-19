#ifndef _WIFIMANAGER_H
#define _WIFIMANAGER_H

#include "WiFiRecord.h"

typedef class WiFiManager {
  public:
    WiFiManager(WiFiRecord*);
    WiFiRecord getCredentials(int);
    WiFiRecord getCredentials(String);
    void connect(WiFiCredentials);
    void connect(int);
    void connect(String);
    bool isConnected();
   private:
    WiFiRecord* _credentials;
} WiFiManager;

#endif
