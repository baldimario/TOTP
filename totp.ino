/*
 * Default power on with left button press
 * Default power off with left button 6 sec press
 * Main button write otp and switch off the device
 * Right button change account
 * Right button long press enter config mode creating a wifi with a captive portal to configure the device
 */
#include <M5StickCPlus.h>
#include "CredentialManager.h"
//#include "WiFiManager.h"
#include "UI.h"
#include "Input.h"
#include "NTP.h"
#include "AXP192.h"
#include <BleKeyboard.h>
#include <Preferences.h>
#define LED G10

#include <WiFi.h>
#include <DNSServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <ArduinoJson.h>

String name = "0x90";
String manufacturer = "localghost";
DNSServer dnsServer;
AsyncWebServer server(80);

BleKeyboard bleKeyboard(name.c_str(), manufacturer.c_str(), 100);

RTC_TimeTypeDef rtc_time;
RTC_DateTypeDef rtc_date;

Credential credentials[100];
UI ui;
Input input;
NTP ntp;
Preferences preferences;
String secretCode = String("");
unsigned long oldTimestamp = 0;
unsigned long timer = 0;
bool configModeEnabled = false;
unsigned long n_credentials = 0;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
    <html>
    <head>
        <title>{{name}}</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
            * {
                padding: 0px;
                margin: 0px;
            }

            body {
                background-color: #111;
                color: white;
            }

            .header {
                background-color: #003e33;
                height: 3em;
                text-align: center;
                padding-top: 1em;
                border-bottom: 1px solid #006e62 
            }

            .json {
                font-size: 1em;
                padding: 2em;
                width: 85%;
                margin-top: 2em;
                height: 45vh;
            }

            .container {
                padding-left: 1em;
                padding-right: 1em;
            }

            .button {
                border-radius: 5px;
                border: 1px solid #003e33;
                background-color: #006856;
                color: white;
                padding: 2em;
                font-size: 1em;
                font-weight: bold;
                text-transform: uppercase;
                display: block;
                width: 100%;
                margin-top: 2em;
            }
        </style>
    </head>
    <body>
        <nav class="header">
            <h1>{{name}}</h1>
        </nav>

        <div class="container">
            <form action="/setRTC">
                <input id="timestamp" type="hidden" name="timestamp" value="">
                <input class="button" type="submit" value="Set RTC">
            </form>

            <form action="/resetSecrets">
                <input class="button"  type="submit" value="Reset Credentials">
            </form>

            <form action="/setSecrets">
                <textarea class="json" type="text" name="data">
{{jsonData}}
                </textarea>
                <input class="button"  type="submit" value="Set Credentials">
            </form>
        </div>

        <script>
            function setTimestamp() {
                document.getElementById('timestamp').value = Math.floor(new Date().getTime()/1000);
            }

            setInterval(setTimestamp, 100);
            setTimestamp();
        </script>
    </body>
</html>)rawliteral";

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    String jsonData = preferences.getString("data", "{\"count\":0,\"list\":[]}");
    String renderedHTML = String(index_html);
    renderedHTML.replace("{{jsonData}}", jsonData);
    renderedHTML.replace("{{name}}", name);
    request->send_P(200, "text/html", renderedHTML.c_str()); 
  }
};

void setupServer(){
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      String jsonData = preferences.getString("data", "{\"count\":0,\"list\":[]}");
      String renderedHTML = String(index_html);
      renderedHTML.replace("{{jsonData}}", jsonData);
      renderedHTML.replace("{{name}}", name);
      request->send_P(200, "text/html", renderedHTML.c_str()); 
      Serial.println("Client Connected");
  });
    
  server.on("/setRTC", HTTP_GET, [] (AsyncWebServerRequest *request) {
      unsigned long timestamp;
      
      if (request->hasParam("timestamp")) {
        timestamp =  atol(String(request->getParam("timestamp")->value()).c_str())+1;
        ntp.setRTC(timestamp);
      }

      request->send(200, "text/html", "The Settings was successfully applied <br><a href=\"/\">Return to Home Page</a>");
      
      configModeEnabled = false;
      delay(2000);
      M5.Axp.PowerOff();
  });

  server.on("/getSecrets", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String jsonData = preferences.getString("data", "{\"count\":0,\"list\":[]}");
    request->send(200, "text/html", jsonData);
  });

  
  server.on("/resetSecrets", HTTP_GET, [] (AsyncWebServerRequest *request) {
    preferences.putString("data", String("{\"count\":0,\"list\":[]}"));
    request->send(200, "text/html", "Ok");
    delay(2000);
    M5.Axp.PowerOff();
  });
  
  server.on("/setSecrets", HTTP_GET, [] (AsyncWebServerRequest *request) {
    if (request->hasParam("data")) {        
      preferences.putString("data", String(request->getParam("data")->value()).c_str());
    }
    
    request->send(200, "text/html", "Ok");
    delay(2000);
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
  WiFi.softAP(name.c_str());
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  setupServer();
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); //only when requested from AP
  server.begin();
}

void setup() {
  Serial.begin(115200);
  
  preferences.begin("storage", false);

  String json = preferences.getString("data", "{\"list\":[]}");
  Serial.println(json);
  DynamicJsonDocument doc(4096);
  deserializeJson(doc, json);
  JsonObject root = doc.as<JsonObject>();

  Serial.println(root["list"].size());

  n_credentials = root["list"].size();
  //Credential credentials[n_credentials]; //(Credential*) malloc(root["list"].size()*sizeof(Credential));

  for(int i = 0; i < n_credentials; i++) {
    Serial.println(root["list"][i]["name"].as<String>());
    Serial.println(root["list"][i]["secret"].as<String>());
    Serial.println(root["list"][i]["isOTP"].as<bool>());
    credentials[i] = Credential(root["list"][i]["name"].as<String>(), root["list"][i]["secret"].as<String>(), root["list"][i]["isOTP"].as<bool>());
  }
  
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  beep();
  M5.begin();
  M5.Axp.begin();
  M5.Axp.EnableCoulombcounter();
  //M5.Imu.Init();
  bleKeyboard.begin();
  ui.init();
  //wifiManager.connect(1);
  ui.setTextSize(4);
  ui.infoCenter(name);
  ui.setTextSize(2);  
  ui.draw();

  beep();
  delay(500);
  //webserver.init(dnsServer);
  ui.clear();
  beep();
  delay(100);
  M5.Imu.Init();
}

float theta = 0;
float phi = 0;
float last_theta = 0;
float last_phi = 0;
long counter = 0;
void loop() {
  M5.update();
  ui.clear();
  dnsServer.processNextRequest();
  //webserver.update(dnsServer);
  
  float accX = 0;
  float accY = 0;
  float accZ = 0;
  M5.Imu.getAccelData(&accX, &accY, &accZ);
  if ((accX < 1) && (accX > -1)) {
      theta = asin(-accX) * 57.295;
  }
  if (accZ != 0) {
      phi = atan(accY / accZ) * 57.295;
  }
        
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
    configModeEnabled = true;
    //webserver.start(server, caprivePortalHandler);
  }

  if(configModeEnabled) {
    ui.infoCenter("Config Mode");
    ui.draw();
    return;
  }

  if (input.btnAisPressed()) {
    beep();
    bleKeyboard.print(secretCode + "\n");
    delay(500);
    beep();
    delay(100);
    beep();
    delay(1000);
    M5.Axp.PowerOff();
  }

  if (timestamp != oldTimestamp) { 
    if(n_credentials > 0) {
      if(last_phi > 10) {
        counter++;
        beep();
        timer = 0;
      }
      if(last_phi < -10) {
        counter--;
        beep();
        timer = 0;
      }
      //long idx = input.getValue() % n_credentials;
      long idx = (counter+input.getValue()) % n_credentials;
      Credential credential = credentials[idx];
      secretCode = credential.getSecret();
      bool isOTP = credential.isOTP();
      String name = credential.getName();
      String displayCode = isOTP ? secretCode : "*****";
  
      ui.fillRect(0, 0, 240, 40, DARKCYAN);
      ui.rect(0, 41, 240, 1, WHITE);
  
      ui.setTextSize(2);
      ui.setColor(bleKeyboard.isConnected() ? GREEN : RED);
      ui.info(bleKeyboard.isConnected() ? "Connected" : "!Connected", 1, 20);
      
      ui.setColor(isCharging() ? GREEN : RED);
      ui.infoTopLeft(isCharging() ? String("Charging") : String((int)getBatteryLevel()) + "%");

  
      ui.setColor(WHITE);
      ui.setTextSize(3);
      ui.infoCenter(isOTP ? name + ":" + displayCode : name, 12);
      
      ui.setTextSize(2);
      ui.setColor(CYAN);
      ui.printDate();
    
      if(isOTP) {
        int progress = (((float)( (timestamp) % 30 ) / 30) ) * 100;
        ui.setColor(WHITE);
        ui.progress(progress);
      }
      else {
        ui.footer("Hidden password");
      }
    }
    else {
      ui.infoCenter("No Config");
      ui.clear();
      ui.draw();
      delay(3000);
      beep();
      setupWiFi();
      configModeEnabled = true;
    }
    ui.draw();
    timer++;
  }

  if(timer > 75) {
    beep();
    M5.Axp.PowerOff();
  }

  last_theta = theta;
  last_phi   = phi;
  oldTimestamp = timestamp;
}
