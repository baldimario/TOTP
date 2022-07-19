#include "ConfigWebServer.h"
#include "NTP.h"

ConfigWebServer::ConfigWebServer() {}

void ConfigWebServer::init(DNSServer dnsServer){
  WiFi.mode(WIFI_AP); 
  WiFi.softAP("TOTP");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  
  dnsServer.start(53, "*", WiFi.softAPIP());
}

void ConfigWebServer::start(AsyncWebServer server, CaptiveRequestHandler* captivePortalHandler) {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send_P(200, "text/html", index_html); 
      Serial.println("Client Connected");
  });
    
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
      unsigned long timestamp;
      
      if (request->hasParam("timestamp")) {
        timestamp =  atol(String(request->getParam("timestamp")->value()).c_str())+3;
        NTP ntp;
        ntp.setRTC(timestamp);
      }

      request->send(200, "text/html", "The RTC was successfully set <br><a href=\"/\">Return to Home Page</a>");
      
      delay(1000);
      M5.Axp.PowerOff();
  });
  
  server.addHandler(captivePortalHandler).setFilter(ON_AP_FILTER);//only when requested from AP
  server.begin();
}

void ConfigWebServer::update(DNSServer dnsServer) {
  dnsServer.processNextRequest();
}
