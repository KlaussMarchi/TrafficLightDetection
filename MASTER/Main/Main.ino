// INCLUSÃO DE BIBLIOTECAS
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <Update.h>
#include <FS.h>
#include <LittleFS.h>
#include <esp_wifi.h>

#define RELAY_PIN 2
#define BUZZER_PIN 0

#define BUZZER_NO_SOUND 0
#define BUZZER_BLOW 2800
#define BUZZER_BAD_SOUND 500
#define BUZZER_GOOD_SOUND 2800

// INICIALIZAÇÃO DE OBJETOS
Preferences preferences;
WebServer server(80);

// VARIÁVEIS GLOBAIS - GERAL
String serverURL = "http://192.168.1.18:8000/";
StaticJsonDocument<500> settings;
int volume = 255;
bool is_autonomous;
unsigned long autoTime;
byte currentLight;
int redTime;
int greenTime;
int yellowTime;


void setup(){
    Serial.begin(115200);
    storageSetup();
    pinSetup();
    wifiSetup();
}

void loop(){
    checkBgTasks();
}
