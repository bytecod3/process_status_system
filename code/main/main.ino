#include <WiFi.h>
#include <AsyncTCP.h>
#include<ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include "credentials.h"
#include <Arduino_JSON.h>

// pin assignments
String process1_led_state = "OFF";
const int process1_led = 2; 

String process2_led_state = "OFF";
const int process2_led = 4;

// string to hold the received message
String message = "";

// servo pin
const int servo_motor_pin = 5;
String servo_angle = "0";

int servo_angle_to_set;

// create web server object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws"); // this object called ws handles connections on the '/ws' path

// variable to hold servo angle value
JSONVar process_data;

String getServoValues(){
  // get the servo angle
  process_data["servo_value"] = String(servo_angle);
  
  // LED states
  process_data["led1state"] = String(process1_led_state);
  process_data["led2state"] = String(process2_led_state);
  
  String process_states = JSON.stringify(process_data);
  return process_states;
}

void notifyClients(String servo_angles){
  ws.textAll(String(process1_led_state));
  ws.textAll(String(process2_led_state));
  ws.textAll(String(servo_angles));

}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
	
	message = (char*)data;

    // check the message received depending on what was sent from the web page
     if(strcmp((char*)data, "led1on") == 0){
		digitalWrite(process1_led, HIGH);
        process1_led_state = "ON";
        notifyClients(getServoValues());
      }
	  
	  if(strcmp((char*)data, "led1off") == 0){
		digitalWrite(process1_led, LOW);
        process1_led_state = "OFF";
        notifyClients(getServoValues());
      }
	  
	  if(strcmp((char*)data, "led2on") == 0){
		digitalWrite(process2_led, HIGH);
        process2_led_state = "ON";
        notifyClients(getServoValues());
      }
	  
	  if(strcmp((char*)data, "led2off") == 0){
		digitalWrite(process2_led, LOW);
        process2_led_state = "OFF";
        notifyClients(getServoValues());
      }
	  
	  if(strcmp((char*)data, "servo") >= 0){
		  // update servo angle => message looks like this: "servo30"
		  servo_angle = message.substring(5); 
      
		  servo_angle_to_set = servo_angle.toInt();

     // write to servo here     

		  Serial.println(servo_angle_to_set);
          notifyClients(getServoValues());
      }
	  
	  // if current servo value is requested
	  if(strcmp((char*)data, "get_servo_angle") == 0){
		notifyClients(getServoValues());
	  }
      
  }
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

void initSPIFFS(){
   // initialize spiffs filesystem
  if(!SPIFFS.begin(true)){
    Serial.println("Error mounting SPIFFS");
  }else{
	Serial.println("SPIFFS mount sucess!");
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

  // Serial port for debugging 
  Serial.begin(115200);

  // configure LED pins as output
  pinMode(process1_led , OUTPUT);
  pinMode(process2_led , OUTPUT);

  // set all LEDs LOW initially
  digitalWrite(process1_led , LOW);
  digitalWrite(process1_led , LOW);
  
  // mount file system
  initSPIFFS();
  
  initWIFI();
  
  initWebSocket();
	
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });
  
  //serve static files from server
  server.serveStatic("/", SPIFFS, "/");
  
  // Start server
  server.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  
  //analogWrite(servo_motor_pin);

  ws.cleanupClients();
}
