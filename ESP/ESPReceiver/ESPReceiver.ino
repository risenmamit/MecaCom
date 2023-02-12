#include <WiFi.h>              // ESP32 WiFi Library
#include <WebServer.h>         // WebServer Library for ESP32
#include <WebSocketsClient.h>  // WebSocket Client Library for WebSocket
#include <ArduinoJson.h>       // Arduino JSON Library
#include <string.h>


const char* ssid = "maheshwari";        // Wifi SSID
const char* password = "maheshwari65";  //Wi-FI Password
WebSocketsClient webSocket;             // websocket client class instance
StaticJsonDocument<100> doc;            // Allocate a static JSON document

int front_left = 27;
int front_right = 13;
int rear_left = 32;
int rear_right = 17;

// F: Front
// R: Rear
int FR_Channel = 0; 
int FL_Channel = 1;
int RR_Channel = 2;
int RL_Channel = 3;

TaskHandle_t Task1;
TaskHandle_t Task2;

volatile int whattodo = 5;


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
  webSocket.begin("192.168.0.108", 3000);
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
    vTaskDelay(50);  //breather
  }
}

void motorHandeler(void* pvParameters) {
  for (;;){
    if (whattodo == 8){
      ledcWrite(FL_Channel, stop + (max_forward-stop)/2);
      ledcWrite(FR_Channel, stop + (max_forward-stop)/2);
      ledcWrite(RL_Channel, stop + (max_forward-stop)/2);
      ledcWrite(RR_Channel, stop + (max_forward-stop)/2);
    }
    else if (whattodo == 5){
      ledcWrite(FL_Channel, stop);
      ledcWrite(FR_Channel, stop);
      ledcWrite(RL_Channel, stop);
      ledcWrite(RR_Channel, stop);
    }
    else if (whattodo == 2){
      ledcWrite(FL_Channel, 204.755);//stop + (max_backward-stop)/2
      ledcWrite(FR_Channel, 204.755);
      ledcWrite(RL_Channel, 204.755);
      ledcWrite(RR_Channel, 204.755);
    }
    else if (whattodo == 4){
      ledcWrite(FR_Channel, stop + (max_forward-stop)/2);
      ledcWrite(FL_Channel, 204.755);
      ledcWrite(RR_Channel, 204.755);
      ledcWrite(RL_Channel, stop + (max_forward-stop)/2);
    }
    else if (whattodo == 6){
      ledcWrite(FL_Channel, stop + (max_forward-stop)/2);
      ledcWrite(FR_Channel, 204.755);
      ledcWrite(RL_Channel, 204.755);
      ledcWrite(RR_Channel, stop + (max_forward-stop)/2);
    }
    else if (whattodo == 7){
      ledcWrite(FR_Channel, stop);
      ledcWrite(RL_Channel, stop);
      ledcWrite(FL_Channel, stop + (max_forward-stop)/2);
      ledcWrite(RR_Channel, stop + (max_forward-stop)/2);
    }
    else if (whattodo == 9){
      ledcWrite(FL_Channel, stop);
      ledcWrite(RR_Channel, stop);
      ledcWrite(FR_Channel, stop + (max_forward-stop)/2);
      ledcWrite(RL_Channel, stop + (max_forward-stop)/2);
    }
    else if (whattodo == 3){
      ledcWrite(FL_Channel, stop);
      ledcWrite(RR_Channel, stop);
      ledcWrite(FR_Channel, 204.755);
      ledcWrite(RL_Channel, 204.755);
    }
    else if (whattodo == 1){
      ledcWrite(FR_Channel, stop);
      ledcWrite(RL_Channel, stop);
      ledcWrite(FL_Channel, 204.755);
      ledcWrite(RR_Channel, 204.755);
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
      Serial.println("recived");
      if (strcmp(doc["conection"].as<String>().c_str(), "message") == 0) {
        Serial.print("whattodo: ");
        whattodo = doc["message"].as<int>();
        Serial.println(whattodo);
      }
    }
    //Serial.print(doc["conection"].as<char[]>());
  }
  if (type == WStype_DISCONNECTED) {
    Serial.print("\nDisconnected\n");
    //webSocket.begin("ws://292.168.212.42", 3000);
  }
}