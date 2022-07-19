/*
 * Default power on with left button press
 * Default power off with left button 6 sec press
 * Main button write otp and switch off the device
 * Right button change account
 * Right button long press enter config mode creating a wifi with a captive portal to configure the device
 */
#include <M5StickCPlus.h>
#include "OTPManager.h"
#include "WiFiManager.h"
#include "UI.h"
#include "Input.h"
#include "NTP.h"
#include "AXP192.h"
//#include "ConfigWebServer.h"
#include <BleKeyboard.h>
#define LED G10

#include <WiFi.h>
#include <DNSServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>


DNSServer dnsServer;
AsyncWebServer server(80);

BleKeyboard bleKeyboard;

RTC_TimeTypeDef rtc_time;
RTC_DateTypeDef rtc_date;

OTP totps[] = {
  OTP("VPN", "secret1"),
  OTP("AWS", "secret2")
};

WiFiRecord wifiConnections[] = {
  WiFiRecord("Vodafone-M4MM7F077", "pass1"),
  WiFiRecord("FASTWEB-YDEB1J", "pass2")
};

WiFiManager wifiManager = WiFiManager(wifiConnections);
OTPManager otpManager = OTPManager(totps);
UI ui;
Input input;
NTP ntp;
String totpCode = String("");
unsigned long oldTimestamp = 0;
unsigned long timer = 0;
bool configModeEnabled = false;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>TOTP</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <h3>TOTP</h3>
  <br><br>
  <form action="/get">
    <input id="timestamp" type="text" name="timestamp" value="">
    <input type="submit" value="Set Timestamp">
  </form>
  <script>
    function setTimestamp() {
      document.getElementById('timestamp').value = Math.floor(new Date().getTime()/1000);
    }

    setInterval(setTimestamp, 1000);
    setTimestamp();
  </script>
</body></html>)rawliteral";

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html); 
  }
};

void setupServer(){
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send_P(200, "text/html", index_html); 
      Serial.println("Client Connected");
  });
    
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
      unsigned long timestamp;
      
      if (request->hasParam("timestamp")) {
        timestamp =  atol(String(request->getParam("timestamp")->value()).c_str())+3;
        ntp.setRTC(timestamp);
      }

      request->send(200, "text/html", "The RTC was successfully set <br><a href=\"/\">Return to Home Page</a>");
      
      configModeEnabled = false;
      delay(1000);
      M5.Axp.PowerOff();
  });
}
 

float getBatteryCurrent() {
  M5.Axp.GetBatCurrent();
}

bool isCharging() {
  return M5.Axp.GetBatPower() == 0;
}

float getBatteryVoltage() {
  return M5.Axp.GetVbatData() * 1.1 / 1000;
}

float getBatteryCapacity() {
  return M5.Axp.GetVapsData() * 1.4 / 1000;
}

float getBatteryLevel() {
  int level = 100.0 * ((getBatteryCapacity() - 3.0) / (getBatteryVoltage() - 3.0));
  return level > 100 ? 100 : level;
}


void beep() {
  M5.Beep.tone(4000);
  digitalWrite(LED, isCharging());
  delay(25);
  M5.Beep.mute();
  digitalWrite(LED, !isCharging());
}

void setupWiFi() {
  WiFi.mode(WIFI_AP); 
  WiFi.softAP("TOTP");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  setupServer();
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  server.begin();
}

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  beep();
  M5.begin();
  M5.Axp.begin();
  M5.Axp.EnableCoulombcounter();
  M5.Imu.Init();
  ui.init();
  //wifiManager.connect(1);
  ui.setTextSize(4);
  ui.infoCenter("TOTP");
  ui.setTextSize(2);  
  ui.draw();

  delay(500);
  
  //webserver.init(dnsServer);
  beep();
  ui.clear();
  bleKeyboard.begin();
}

void loop() {
  M5.update();
  ui.clear();
  dnsServer.processNextRequest();
  //webserver.update(dnsServer);

  unsigned long timestamp = ntp.getEpoch(); 
    
  digitalWrite(LED, !isCharging());
  
  if (input.btnBisPressed()) {
    input.updateValue();
    beep();
    timer = 0;
  }

  if (input.btnBisPressed(1000)) {
    beep();
    setupWiFi();
    //webserver.start(server, caprivePortalHandler);
    configModeEnabled = true;
  }

  if(configModeEnabled) {
    ui.infoCenter("Config");
    ui.draw();
    return;
  }

  if (input.btnAisPressed()) {
    beep();
    bleKeyboard.print(totpCode + "\n");
    beep();
    delay(1000);
    M5.Axp.PowerOff();
  }

  if (timestamp != oldTimestamp) {    
    long idx = input.getValue() % 2;
    totpCode = otpManager.getOTP(idx).getToken(timestamp);

    ui.fillRect(0, 0, 240, 40, DARKCYAN);
    ui.rect(0, 41, 240, 1, WHITE);
    ui.setColor(bleKeyboard.isConnected() ? GREEN : RED);
    ui.info(bleKeyboard.isConnected() ? "Connected" : "!Connected", 1, 20);
    
    //Serial.println(ntp.getEpoch());
    ui.setColor(isCharging() ? GREEN : RED);
    ui.infoTopLeft(isCharging() ? String("Charging") : String((int)getBatteryLevel()) + "%");
    ui.setColor(WHITE);
    ui.setTextSize(3);
    ui.infoCenter(otpManager.getOTP(idx).getName() + ":" + totpCode, 12);
    ui.setTextSize(2);
    
    ui.setColor(CYAN);
    ui.printDate();
    
    int progress = (((float)( (timestamp) % 30 ) / 30) ) * 100;
    ui.setColor(WHITE);
    ui.progress(progress);
    ui.draw();
    timer++;
  }

  if(timer > 75) {
    beep();
    M5.Axp.PowerOff();
  }

  oldTimestamp = timestamp;
}
