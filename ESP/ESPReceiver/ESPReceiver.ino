#include <WiFi.h> // ESP32 WiFi Library
#include <WebServer.h> // WebServer Library for ESP32
#include <WebSocketsClient.h> // WebSocket Client Library for WebSocket
#include <ArduinoJson.h> // Arduino JSON Library
#include <string.h>


const char* ssid = "F19"; // Wifi SSID
const char* password = "1234robo"; //Wi-FI Password
WebSocketsClient webSocket; // websocket client class instance
StaticJsonDocument<100> doc; // Allocate a static JSON document

int front_left = 5;
int front_right = 18;
int rear_left = 19;
int rear_right = 21;

int FL_Channel = 0;
int FR_Channel = 1;
int RL_Channel = 2;
int RR_Channel = 3;

const int freq = 490;
const int resolution = 16;

TaskHandle_t Task1;
TaskHandle_t Task2;

void setup() {
  // put your setup code here, to run once:
  ledcSetup(FL_Channel, freq, resolution);
  ledcAttachPin(front_left, FL_Channel);

  ledcSetup(FR_Channel, freq, resolution);
  ledcAttachPin(front_right, FR_Channel);
  
  ledcSetup(RL_Channel, freq, resolution);
  ledcAttachPin(rear_left, RL_Channel);
  
  ledcSetup(RR_Channel, freq, resolution);
  ledcAttachPin(rear_right, RR_Channel);

  
  WiFi.begin(ssid, password);
  Serial.begin(115200);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); // Print local IP address

  //webSocket.setExtraHeaders("user-agent: Mozilla");
  webSocket.begin("192.168.72.201", 3000);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(6000);
  xTaskCreatePinnedToCore(socketLoopHandeler,
                          "Task1",   /* name of task. */
                          10000,     /* Stack size of task */
                          NULL,      /* parameter of the task */
                          1,         /* priority of the task */
                          &Task1,    /* Task handle to keep track of created task */
                          0);

    // xTaskCreatePinnedToCore(motorHandeler,
    //                       "Task2",   /* name of task. */
    //                       10000,     /* Stack size of task */
    //                       NULL,      /* parameter of the task */
    //                       1,         /* priority of the task */
    //                       &Task2,    /* Task handle to keep track of created task */
    //                       1);    
}

void loop() {
  
}

void socketLoopHandeler(void * pvParameters){
  for(;;){
    webSocket.loop();
    vTaskDelay(10); //breather
  } 
}

void motorHandeler(void * pvParameters){
  for(;;);
}
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  if(type == WStype_CONNECTED){
    DynamicJsonDocument doc(1024);
    char msg[100];
    doc["isfirst"] = "true";
    doc["device"] = "ESP";
    serializeJson(doc, msg);
    webSocket.sendTXT(msg);
    Serial.println(msg);
  }
  if (type == WStype_TEXT)
  {
    // deserialize incoming Json String
    DeserializationError error = deserializeJson(doc, payload); 
    if (error) { // Print erro msg if incomig String is not JSON formated
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }
    else{
      Serial.print("Connection: ");
      serializeJson(doc["conection"], Serial);
      Serial.print("\n");
      if(strcmp(doc["conection"].as<String>() .c_str(), "message" ) == 0){
        if(strcmp(doc["message"].as<String>() .c_str(), "F") == 0){
          Serial.println("Forward");
        }
        else if(strcmp(doc["message"].as<String>() .c_str(), "R") == 0){
          Serial.println("Backward");
          
        }
      }
    }
    //Serial.print(doc["conection"].as<char[]>());
  }
  if(type == WStype_DISCONNECTED){
    Serial.print("\nDisconnected\n");
    //webSocket.begin("ws://192.168.0.108", 3000);
  }
}
