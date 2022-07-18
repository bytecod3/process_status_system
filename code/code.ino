#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include<ESPAsyncWebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include <Arduino_JSON.h>
#include <Servo.h>
#include <DHT.h>
#include "credentials.h"

#define SERVO_DELAY 50

// LED PINS
String process1_led_state = "OFF";
const int process1_led = 13; 

String process2_led_state = "OFF";
const int process2_led = 0;

String process3_led_state = "OFF";
const int process3_led = 4;

String process4_led_state = "OFF";
const int process4_led = 5;

// DHT11 temperature sensor pin
#define DHTPIN 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float temperature;

// LDR pin
int ldr_pin = A0;
int light_intensity;

// servo pin
Servo servo_motor;
int servo_motor_pin = 16;
int angle;
String servo_angle = "0";

int servo_angle_to_set;

// create web server object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

JSONVar process_data;
String message;


void notifyClients(String s){
  ws.textAll(String(s));
}

String getProcessData(){
  // LED states
  process_data["led1state"] = String(process1_led_state);
  process_data["led2state"] = String(process2_led_state);
  process_data["led3state"] = String(process3_led_state);
  process_data["led4state"] = String(process4_led_state);

  process_data["ambient-light"] = String(getLight().c_str());
  process_data["servo-angle"] = String(getAngle().c_str());

  // convert to JSON
  String process_states = JSON.stringify(process_data);
  Serial.println(process_states);

  return process_states;
}

// listen for web socket messages
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    message = (char*)data;

    // check the message received depending on what was sent from the web page
     if(strcmp((char*)data, "led1on") == 0){
        digitalWrite(process1_led, HIGH);
        process1_led_state = "ON";
        notifyClients(getProcessData());
      }
    
    if(strcmp((char*)data, "led1off") == 0){
        digitalWrite(process1_led, LOW);
        process1_led_state = "OFF";
        notifyClients(getProcessData());
      }
    
    if(strcmp((char*)data, "led2on") == 0){
        digitalWrite(process2_led, HIGH);
        process2_led_state = "ON";
        notifyClients(getProcessData());
      }
    
    if(strcmp((char*)data, "led2off") == 0){
        digitalWrite(process2_led, LOW);
        process2_led_state = "OFF";
        notifyClients(getProcessData());
      }

    if(strcmp((char*)data, "led3on") == 0){
      digitalWrite(process3_led, HIGH);
      process3_led_state = "ON";
      notifyClients(getProcessData());
    }

    if(strcmp((char*)data, "led3off") == 0){
      digitalWrite(process3_led, LOW);
      process3_led_state = "OFF";
      notifyClients(getProcessData());
    }

   if(strcmp((char*)data, "led4on") == 0){
      digitalWrite(process4_led, HIGH);
      process4_led_state = "ON";
      notifyClients(getProcessData());
    }

    if(strcmp((char*)data, "led4off") == 0){
      digitalWrite(process4_led, LOW);
      process4_led_state = "OFF";
      notifyClients(getProcessData());
    }

    if(strcmp((char*)data, "get-ambient-light") == 0){
      getLight();
      
      notifyClients(getProcessData());
    }

    if(strcmp((char*)data, "servo") >= 0){
      // update servo angle => message looks like this: "servo30"
      servo_angle = message.substring(5); 
      
      servo_angle_to_set = servo_angle.toInt();

      Serial.println(servo_angle_to_set);
     
      for(int i = 0; i < servo_angle_to_set; i++  ){
          servo_motor.write(i);
          delay(SERVO_DELAY);
      }
    
    }

    
  }
  
}


String getLight(){
  // get current light intensity value
  // read ldr pin
  light_intensity = analogRead(ldr_pin);       
  return String(light_intensity);
        
}

String getAngle(){
  // get current servo motor angle
  angle = servo_motor.read();       
  return String(angle);
        
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
      
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
      
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
      
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void initLittleFS(){
   // initialize spiffs filesystem
  if(!LittleFS.begin()){
    Serial.println("Error mounting LittleFS");
  }else{
  Serial.println("LittleFS mount success!");
  }
}

void initWIFI(){
  // Connect to Wi-Fi
  WiFi.begin(SSID, PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to wi-fi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

}

void setup() {
  // put your setup code here, to run once:

  // pinmodes
  pinMode(process1_led , OUTPUT);
  pinMode(process2_led , OUTPUT);
  pinMode(process3_led , OUTPUT);
  pinMode(process4_led , OUTPUT);

  // configure LDR pin as input
  pinMode(ldr_pin, INPUT);

  // attach servo motor pin
  servo_motor.attach(servo_motor_pin);
  servo_motor.write(0);

  Serial.begin(9600);
  initLittleFS();
  initWIFI();
  initWebSocket();

  // Route for root web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html");
  });

  server.serveStatic("/", LittleFS, "/");
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

  ws.cleanupClients();

}
