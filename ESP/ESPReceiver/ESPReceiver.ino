#include <WiFi.h>              // ESP32 WiFi Library
#include <WebServer.h>         // WebServer Library for ESP32
#include <WebSocketsClient.h>  // WebSocket Client Library for WebSocket
#include <ArduinoJson.h>       // Arduino JSON Library
#include <string.h>


const char* ssid = "maheshwari";        // Wifi SSID
const char* password = "maheshwari65";  //Wi-FI Password
WebSocketsClient webSocket;             // websocket client class instance
StaticJsonDocument<100> doc;            // Allocate a static JSON document

int front_left = 5;
int front_right = 18;
int rear_left = 19;
int rear_right = 21;

int FL_Channel = 0;
int FR_Channel = 1;
int RL_Channel = 2;
int RR_Channel = 3;

TaskHandle_t Task1;
TaskHandle_t Task2;

volatile int whattodo = 0;

int cmd;

const int maxRPM = 370;// min
const float maxRPS = maxRPM/60;
const float radius=7.6; // radius of wheel cm
const float circum=2*3.14*radius;// circumference of the wheel cm
const float maxVel = circum*maxRPS;// cm/sec
const int resolution = 12;
const float freq = 50; //each incrimet should be 3ms
unsigned long int Pmax = pow(2, resolution) - 1; //max pwm duty cycle value
unsigned long int max_forward = Pmax*0.002*freq; //0.002 is 2000us
unsigned long int stop = Pmax*0.0015*freq; //0.0015 is 1500us
unsigned long int max_backward = Pmax*0.001*freq; //0.001 is 1000us
unsigned long int stepDelay = pow(10, 6)/freq+100;// 100 for ensuring signal s

int vtop(float vel)//velocity to pulse
{
  unsigned long int pul;
  float RPS = vel/circum;
  pul=(max_forward-max_backward)*(RPS+maxRPS)/(2*maxRPS) + max_backward;
  return pul;
}

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
  Serial.println(WiFi.localIP());  // Print local IP address

  //webSocket.setExtraHeaders("user-agent: Mozilla");
  webSocket.begin("192.168.0.107", 3000);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(6000);
  xTaskCreatePinnedToCore(socketLoopHandeler,
                          "Task1", /* name of task. */
                          10000,   /* Stack size of task */
                          NULL,    /* parameter of the task */
                          1,       /* priority of the task */
                          &Task1,  /* Task handle to keep track of created task */
                          0);

  xTaskCreatePinnedToCore(motorHandeler,
                        "Task2",   /* name of task. */
                        10000,     /* Stack size of task */
                        NULL,      /* parameter of the task */
                        1,         /* priority of the task */
                        &Task2,    /* Task handle to keep track of created task */
                        1);
}

void loop() {
}

void socketLoopHandeler(void* pvParameters) {
  for (;;) {
    webSocket.loop();
    vTaskDelay(100);  //breather
  }
}

void motorHandeler(void* pvParameters) {
  int i = 0;
  for (;;){
    i = i +1;
    if (i == 1000){
      Serial.print("motorHandeler: ");
      Serial.println(whattodo);
      i = 0;
    }      
    if (whattodo == 1){
      ledcWrite(0, max_forward);
      ledcWrite(1, max_forward);
      ledcWrite(2, max_forward);
      ledcWrite(3, max_forward);
    }
    else if (whattodo == 0){
      ledcWrite(0, stop);
      ledcWrite(1, stop);
      ledcWrite(2, stop);
      ledcWrite(3, stop);
    }
    else if (whattodo == -1){
      ledcWrite(0, max_backward);
      ledcWrite(1, max_backward);
      ledcWrite(2, max_backward);
      ledcWrite(3, max_backward);
    }
  }
}
void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  if (type == WStype_CONNECTED) {
    DynamicJsonDocument doc(1024);
    char msg[100];
    doc["isfirst"] = "true";
    doc["device"] = "ESP";
    serializeJson(doc, msg);
    webSocket.sendTXT(msg);
    
    Serial.println(msg);
  }
  if (type == WStype_TEXT) {
    // deserialize incoming Json String
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {  // Print erro msg if incomig String is not JSON formated
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    } else {
      Serial.print("Connection: ");
      serializeJson(doc["conection"], Serial);
      Serial.print("\n");
      if (strcmp(doc["conection"].as<String>().c_str(), "message") == 0) {
        serializeJson(doc["message"], Serial);
        cmd = doc["message"];
        if (strcmp(doc["message"].as<String>().c_str(), "F") == 0) {
          Serial.println("Forward");
          whattodo = 1;
          Serial.print("Control: ");
          Serial.println(whattodo);
        } else if (strcmp(doc["message"].as<String>().c_str(), "R") == 0) {
          Serial.println("Backward");
          whattodo = -1;
          Serial.print("Control: ");
          Serial.println(whattodo);
        }else if (strcmp(doc["message"].as<String>().c_str(), "S") == 0) {
          Serial.println("Stop");
          whattodo = 0;
          Serial.print("Control: ");
          Serial.println(whattodo);
        }
      }
    }
    //Serial.print(doc["conection"].as<char[]>());
  }
  if (type == WStype_DISCONNECTED) {
    Serial.print("\nDisconnected\n");
    //webSocket.begin("ws://192.168.0.108", 3000);
  }
}