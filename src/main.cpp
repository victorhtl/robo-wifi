#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <LITTLEFS.h>
#include "joystick.h"
#include "wifi_manager.h"

/* Joystick protocol Constants */
const char init_char = '{';
const char middle_char = ',';
const char end_char = '}';
const uint8_t maxBufferSize = 9;

/* Wifi Manager constants */
String ssid;
String pass;
String ip;

const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pass";
const char* PARAM_INPUT_3 = "ip";

IPAddress localIP;
IPAddress localGateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

bool restart = false;

/* Filesystem constants */
const char* ssidPath = "/ssid.txt";
const char* passPath = "/pass.txt";
const char* ipPath = "/ip.txt";

// Motor Controll pins
const uint8_t pwm_in1 = 15;
const uint8_t pwm_in2 = 4;
const uint8_t pwm_in3 = 18;
const uint8_t pwm_in4 = 19;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

char data_joystick[maxBufferSize];
bool data_avaliable = false;
bool set_pwm = false;

bool charIsValid(char c){
  return (c >= '0' and c <= '9') or c == init_char or c == end_char or c == middle_char;
}

// WebSock connection manager
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){ 
  if(type == WS_EVT_CONNECT){
    Serial.println("WebSocket Conectado");
  }
  else if(type == WS_EVT_DATA){ 
    char* _data = (char*) data; 

    for(uint8_t i = 0; charIsValid(_data[i]) and i < maxBufferSize; i ++){
      data_joystick[i] = _data[i];
    }                  

    if(data_joystick[0] == init_char and data_joystick[4] == middle_char and data_joystick[8] == end_char){
      data_avaliable = true;
    }
  }
}

void initFS(){
  if(!LittleFS.begin(true)){
    Serial.println();
    Serial.println("An error has occurred while mounting LittleFS");
    Serial.println();
  } else {
    Serial.println("LittleFS mounted successfully");
  }
}

String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- failed to open file for reading");
    return String();
  }

  String fileContent;
  while(file.available()){
    fileContent = file.readStringUntil('\n');
    break;
  }
  file.close();
  return fileContent;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- file closed");
  }
  file.close();
}

void setupPins(){
  pinMode(pwm_in1, OUTPUT);
  pinMode(pwm_in2, OUTPUT);
  pinMode(pwm_in3, OUTPUT);
  pinMode(pwm_in4, OUTPUT);
  digitalWrite(pwm_in1, LOW);
  digitalWrite(pwm_in2, LOW);
  digitalWrite(pwm_in3, LOW);
  digitalWrite(pwm_in4, LOW);
}

bool setupWifi(){
  if((ssid == "") or (ip == "")){
    Serial.println("Undefined SSID");
    return false;
  }

  WiFi.mode(WIFI_STA);
  localIP.fromString(ip.c_str());

  if (!WiFi.config(localIP, localGateway, subnet)){
    Serial.println("STA Failed to configure");
    return false;
  }

  WiFi.begin(ssid.c_str(), pass.c_str());

  Serial.print("Conecting...");
  delay(10000);
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Failed to connect");
    return false;
  }
  Serial.println(" Conected!");
  Serial.println(WiFi.localIP());
  return true;
}

void setup() {
  Serial.begin(115200);

  setupPins();
  initFS();

  ssid = readFile(LittleFS, ssidPath);
  pass = readFile(LittleFS, passPath);
  ip = readFile(LittleFS, ipPath);

  Serial.println("Trying to connect to: ");
  Serial.printf("SSID: %s - PASS %s - IP %s\r\n", ssid, pass, ip);

  if(setupWifi()){
    ws.onEvent(onEvent);
    server.addHandler(&ws);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send_P(200, "text/html", index_html);
    });

    server.begin();

    Serial.println("HTTP Server On!");
  } else {
    
    Serial.println("Setting AP...");
    WiFi.softAP("ESP-WIFI-MANAGER", NULL);
    IPAddress IP = WiFi.softAPIP();
    
    Serial.println(IP);
    
    server.on("/", HTTP_GET, [](AsyncWebServerRequest * request){
      request->send_P(200, "text/html", manager_html);
    });

    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request){
      int params = request->params();
      for(int i = 0; i < params; i++){
        AsyncWebParameter *p = request->getParam(i);
        if(p->isPost()){
          if(p->name() == PARAM_INPUT_1){
            ssid = p->value().c_str();
            writeFile(LittleFS, ssidPath, ssid.c_str());
          }
          if(p->name() == PARAM_INPUT_2){
            pass = p->value().c_str();
            writeFile(LittleFS, passPath, pass.c_str());
          }
          if(p->name() == PARAM_INPUT_3){
            ip = p->value().c_str();
            writeFile(LittleFS, ipPath, ip.c_str());
          }
        }
      }
      restart = true;
      request->send(200, "text/plain", "Done. ESP will restart, connect to your router and go to IP address: " + ip);
    });
    
    server.begin();
  }
}

void loop() {
  if (restart){
    delay(5000);
    ESP.restart();
  }

  char speed_str[5];
  char angle_str[5];

  if(data_avaliable){
    char i = 1;
    char j = 0;

    for(; data_joystick[i] != middle_char; i++, j++){
      speed_str[j] = data_joystick[i];
    }
    speed_str[j] = '\0';
    j = 0;
    i++;
    for(; data_joystick[i] != end_char; i++, j++){
      angle_str[j] = data_joystick[i];
    }
    angle_str[j] = '\0';

    data_avaliable = false;
    set_pwm = true;
  }

  if(set_pwm){
    int speed = atoi(speed_str);
    speed = map(speed, 0, 100, 0, 255);
    int angle = atoi(angle_str);
    uint8_t quadrante;
    uint8_t dutyA, dutyB;

    if(angle < 90) quadrante = 1; 
    else if(angle < 180) quadrante = 2; 
    else if(angle < 270) quadrante = 3; 
    else if(angle < 360) quadrante = 4;
    
    // Controle de velocidade
    if(quadrante == 1 || quadrante == 4) {                
      // se estiver na direita, o motor A tem sua velocidade diminuida pelo cosseno do angulo
      dutyB = (uint8_t) speed;
      dutyA = (uint8_t) (speed * (1.0 - cos(angle * PI / 180.0) / 2.0)); 
    } 
    else {      
      // se estiver na direita, o motor A tem sua velocidade diminuida pelo cosseno do angulo                                                    
      dutyB = (uint8_t) (speed * (1.0 + cos(angle * PI / 180.0) / 2.0));
      dutyA = (uint8_t) speed; 
    }

    if(quadrante < 3) {
      // Para frente
      analogWrite(pwm_in2, dutyB); analogWrite(pwm_in1, 0);
      analogWrite(pwm_in4, dutyA); analogWrite(pwm_in3, 0); 
    }
    else {
      // Para tras
      analogWrite(pwm_in2, 0); analogWrite(pwm_in1, dutyB);
      analogWrite(pwm_in4, 0); analogWrite(pwm_in3, dutyA);  
    }
    set_pwm = false;
  }
}
