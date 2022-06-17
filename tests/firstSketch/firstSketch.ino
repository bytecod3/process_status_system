#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

// set LED GPIO
#define ledPin 15

// store the LED state
String ledState;

const char* ssid = "Gakibia-Unit3";
const char* password = "password";

// create async web server object on port 80
AsyncWebServer server(80);

// Replace placeholder with LED state value
String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
      if(digitalRead(ledPin)){
        ledState = "ON";
      }else{
        ledState = "OFF";
      }

      Serial.print(ledState);
      return ledState;
  }

  return String();
}

void connectToWifi(){
    WiFi.begin(ssid, password);

    while(WiFi.status() != WL_CONNECTED){
        delay(100);
        Serial.println("Connecting...");
    }

    Serial.print("Connected to "); Serial.print(ssid);
    Serial.println();

    Serial.print("IP address:"); Serial.print(WiFi.localIP());
    Serial.println();

}

void setup() {
    Serial.begin(115200);

    // initialize SPIFFS webserver
    if(!SPIFFS.begin(true)){
      Serial.println("Error mounting SPIFFS");
      return;
    }

    // connect to WiFi
    connectToWifi();

    // Routes
    // route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/index.html", String(), false, processor);
    });

    // route to load style.css file
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/style.css", "text.css");
    });

    // start server
    server.begin();

}

void loop() {

}
