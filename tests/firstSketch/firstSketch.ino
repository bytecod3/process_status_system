
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <AsyncJson.h>
#include "wifi_credentials.h"

#define LED_BUILTIN 2

// create async web server object on port 80
AsyncWebServer server(80);

void createWifi(){

    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWORD);

    while(WiFi.waitForConnectResult() != WL_CONNECTED){
          Serial.println("Connection Failed! Rebooting...");
          delay(500);
          ESP.restart();
      }

      Serial.println("Connection successful!");
      MDNS.begin("demo-server");

}

void setup() {
  Serial.begin(115200);
  Serial.println("Booted!");
  createWifi();

  MDNS.begin("demo-server");

  pinMode(LED_BUILTIN, OUTPUT);

  // add default headers to our server to allow access to our server
  DefaultHeaders::Instance().addHeader("Acess-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Acess-Control-Allow-Methods", "GET, PUT");
  DefaultHeaders::Instance().addHeader("Acess-Control-Allow-Headers", "*");

  server.addHandler(new AsyncCallbackJsonWebHandler( "/led",  [ ](AsyncWebServerRequest *request, JsonVariant &json){
    // create a json endpoint called led and pull data from that
      const JsonObject &jsonObj = json.as<JsonObject>();

      if(jsonObj["on"]){
          Serial.println("Turn on LED");
          digitalWrite(LED_BUILTIN, HIGH);
        }else{
          Serial.println("Turn off LED"); 
          digitalWrite(LED_BUILTIN, LOW); 
        }

        request->send(200, "OK"); // okay response
    }));

    // serve files using SPIFFS fileSystem with ndex page as entry point
    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

    server.onNotFound([ ](AsyncWebServerRequest *request){
        if(request->method() == HTTP_OPTIONS){
            request->send(200);
          }else{
            Serial.println("Not found");
            request->send(404, "Not found");
          }
      });


      server.begin();
    
}

void loop() {

}
