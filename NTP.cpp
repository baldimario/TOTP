#include "NTP.h"
#define DEBUG_NTPClient 1

NTP::NTP() {}

void NTP::init() {
  while (!DateTime.isTimeValid()) {
      Serial.println("Failed to get time from server, retry.");
      DateTime.begin();
      delay(500);
  }
  this->setRTC();
}


void NTP::setRTC(unsigned long timestamp) {
  UnixTime stamp(0);
  stamp.getDateTime(timestamp);

  Serial.println(String(stamp.year)+" "+String(stamp.month)+" "+String(stamp.day)+" "+String(stamp.hour)+" "+String(stamp.minute)+" "+String(stamp.second));
  DateTime.setTime(timestamp, true);
  DateTimeParts p = DateTime.getParts();
  RTC_TimeTypeDef timeStruct;
  timeStruct.Seconds = stamp.second;
  timeStruct.Minutes = stamp.minute;
  timeStruct.Hours = stamp.hour;
  M5.Rtc.SetTime(&timeStruct);

  RTC_DateTypeDef DateStruct;
  DateStruct.Date = stamp.day;
  DateStruct.Month = stamp.month;
  DateStruct.Year = stamp.year;
  DateStruct.WeekDay = stamp.dayOfWeek;
  M5.Rtc.SetData(&DateStruct);

  
  
  /*DateTime.setTime(timestamp, true);
  DateTimeParts p = DateTime.getParts();
  RTC_TimeTypeDef timeStruct;
  timeStruct.Seconds = p.getSeconds();
  timeStruct.Minutes = p.getMinutes();
  timeStruct.Hours = p.getHours();
  M5.Rtc.SetTime(&timeStruct);

  RTC_DateTypeDef DateStruct;
  DateStruct.Date = p.getMonthDay();
  DateStruct.Month = p.getMonth()+1;
  DateStruct.Year = p.getYear();
  DateStruct.WeekDay = p.getWeekDay();
  M5.Rtc.SetData(&DateStruct);*/
}

void NTP::setRTC() {
  unsigned long timestamp = DateTime.now();
  this->setRTC();
}

unsigned long NTP::getEpoch() {
  RTC_TimeTypeDef timeStruct;
  RTC_DateTypeDef DateStruct;
  M5.Rtc.GetTime(&timeStruct);
  M5.Rtc.GetData(&DateStruct);
  
  UnixTime stamp(0);
  //Serial.println(String(DateStruct.Year)+" "+String(DateStruct.Month)+" "+String(DateStruct.Date)+" "+String(timeStruct.Hours)+" "+String(timeStruct.Minutes)+" "+String(timeStruct.Seconds));
  stamp.setDateTime(DateStruct.Year, DateStruct.Month, DateStruct.Date, timeStruct.Hours, timeStruct.Minutes, timeStruct.Seconds);
  return stamp.getUnix();
}
