#ifndef _CONFIGWEBSERVER_H
#define _CONFIGWEBSERVER_H

#include <WiFi.h>
#include <DNSServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>TOTP</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <h3>TOTP</h3>
  <br><br>
  <form action="/get">
    <input id="timestamp" type="text" name="timestamp" value="">
    <input type="submit" value="Submit">
  </form>
  <script>
    function setTimestamp() {
      document.getElementById('timestamp').value = Math.floor(new Date().getTime()/1000);
    }

    setInterval(setTimestamp, 1000);
    setTimestamp();
  </script>
</body></html>)rawliteral";


typedef class CaptiveRequestHandler : public AsyncWebHandler {
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
} CaptiveRequestHandler;


typedef class ConfigWebServer {
  public:
    ConfigWebServer();
    void init(DNSServer);
    void start(AsyncWebServer, CaptiveRequestHandler*);
    void update(DNSServer);
  private:
} ConfigWebServer;


#endif
