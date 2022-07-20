#ifndef _UI_H
#define _UI_H

#include <Arduino.h>
#include <M5StickCPlus.h>

typedef class UI {
  public:
    UI();
    void init();
    void info(String, uint8_t, uint8_t);
    void infoCenter(String);
    void infoCenter(String, int);
    void infoLeft(String);
    void infoTopLeft(String);
    void infoTopCenter(String);
    void printDate();
    void printDate(int);
    void setTextSize(uint8_t);
    void setColor(uint16_t);
    void clear();
    void rect(uint8_t, uint8_t, uint8_t, uint8_t);
    void fillRect(uint8_t, uint8_t, uint8_t, uint8_t);
    void rect(uint8_t, uint8_t, uint8_t, uint8_t, uint16_t);
    void fillRect(uint8_t, uint8_t, uint8_t, uint8_t, uint16_t);
    void progress(uint8_t);
    void turnOnDisplay();
    void footer(String);
    void turnOffDisplay();
    void draw();
   private:
    uint8_t _size = 2;
    uint8_t _w = 240;
    uint8_t _h = 135;
    uint16_t _color = WHITE;
    TFT_eSprite _tftSprite = TFT_eSprite(&M5.Lcd);
} UI;

#endif
