#ifndef _NTP_H
#define _NTP_H

#include <Arduino.h>
#include <ESPDateTime.h>
#include <UnixTime.h>
#include <M5StickCPlus.h>

typedef class NTP {
  public:
    NTP();
    void init();
    void setRTC(unsigned long);
    void setRTC();
    void update();
    unsigned long getEpoch();
} NTP;

#endif
