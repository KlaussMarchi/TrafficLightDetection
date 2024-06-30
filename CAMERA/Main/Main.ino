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
#include "esp_camera.h"

// INICIALIZAÇÃO DE OBJETOS
Preferences preferences;
WebServer server(80);

// VARIÁVEIS GLOBAIS - GERAL
String serverURL = "http://192.168.1.18:8000/";
StaticJsonDocument<500> settings;


void setup(){
    Serial.begin(115200);
    storageSetup();
    wifiSetup();
}

void loop(){
    checkBgTasks();
}
