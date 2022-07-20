#include "UI.h";

UI::UI() {}

void UI::init() {
  M5.Axp.ScreenBreath(9);
  M5.Lcd.setRotation(3);
  //M5.Lcd.fillScreen(BLACK);
  this->_tftSprite.createSprite(this->_w, this->_h);
  this->_tftSprite.setRotation(3);
  this->clear();
}

void UI::info(String text, uint8_t x, uint8_t y) {
  this->_tftSprite.setCursor(x, y);
  this->_tftSprite.setTextColor(this->_color);
  this->_tftSprite.setTextSize(this->_size);
  this->_tftSprite.print(text.c_str());
}

void UI::printDate() {
    this->printDate(1);
}

void UI::printDate(int timezone) {
  RTC_TimeTypeDef timeStruct;
  RTC_DateTypeDef DateStruct;
  M5.Rtc.GetTime(&timeStruct);
  M5.Rtc.GetData(&DateStruct);
  //Serial.println(String(DateStruct.Year)+" "+String(DateStruct.Month)+" "+String(DateStruct.Date)+" "+String(timeStruct.Hours)+" "+String(timeStruct.Minutes)+" "+String(timeStruct.Seconds));

  bool isDST = false;
  if(DateStruct.Month > 3 and DateStruct.Month < 10) {
    isDST = true;
  }
  //add day check

  uint8_t offsetHours = (timezone + (isDST ? 1 : 0));

  this->info(String(DateStruct.Date) + "/" + String(DateStruct.Month) + "/" + String(DateStruct.Year), this->_w-8*this->_size*7, 1);
  this->info(String(timeStruct.Hours + offsetHours) + ":" + String(timeStruct.Minutes) + ":" + String(timeStruct.Seconds), this->_w-8*this->_size*7, 20);
}


void UI::infoCenter(String text, int yOffset) {
  int y_offset = -2*this->_size;
  this->info(text, this->_w/2 - ceil(this->_size*6*text.length()/2), this->_h/2 - (this->_size*5/2) + y_offset + yOffset);
}

void UI::infoCenter(String text) {
  this->infoCenter(text, 0);
}

    
void UI::infoLeft(String text) {
  int y_offset = -2*this->_size;
  this->info(text, 0, this->_h/2 - (this->_size*5/2) + y_offset);
}

void UI::infoTopLeft(String text) {
  int y_offset = -2*this->_size;
  this->info(text, 0, 1);
}

void UI::infoTopCenter(String text) {
  int y_offset = -2*this->_size;
  this->info(text, this->_w/2 - (this->_size*5*text.length()/2), 1);
}


void UI::setTextSize(uint8_t size) {
  this->_size = size;
}

void UI::setColor(uint16_t color) {
  this->_color = color;
}

void UI::clear() {
  this->_tftSprite.fillSprite(BLACK);
}

void UI::turnOnDisplay() {
  M5.Axp.SetLDO2(false);
}

void UI::turnOffDisplay() {
  M5.Axp.SetLDO2(true);
}

void UI::rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color) {
  this->_tftSprite.drawRect(x, y, w, h, color);
}

void UI::fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color) {
  this->_tftSprite.fillRect(x, y, w, h, color); 
}

void UI::rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  this->rect(x, y, w, h, WHITE);
}

void UI::fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  this->fillRect(x, y, w, h, WHITE);
}

void UI::progress(uint8_t progress) {
  uint8_t x = 0;
  uint8_t h = 20;
  uint8_t y = this->_h-h;
  uint8_t p = (int) ((float)this->_w*progress/100.0);
  
  this->fillRect(x, y-1, this->_w, h, DARKCYAN);
  this->rect(x, y-2, this->_w-1, h+2, WHITE);
  this->fillRect(x, y-1, p, h, CYAN);
}

void UI::footer(String text) {
  uint8_t x = 0;
  uint8_t h = 20;
  uint8_t y = this->_h-h;
  
  this->fillRect(x, y-1, this->_w, h, DARKCYAN);
  this->rect(x, y-2, this->_w-1, h+2, WHITE);
  this->setTextSize(2);
  this->setColor(WHITE);
  this->info(text, 3, y+2);
}

void UI::draw() {
  this->_tftSprite.pushSprite(0, 0);
}
