// ENVIANDO UM POST COM UM JSON PARA UMA URL ESPECÍFICA 
String postWebData(String URL, String request, const String& jsonString){
    if(WiFi.status() != WL_CONNECTED)
        return "-1";

    Serial.println("\nPOST INICIADO em: " + URL + request);
    Serial.println("Data: " + jsonString);

    HTTPClient http;
    http.begin(URL + request);
    http.addHeader("Content-Type", "application/json"); 
    http.setTimeout(5000);

    int httpCode = http.POST(jsonString);
    String payload = (httpCode > 0) ? http.getString() : "-1";
    
    Serial.println("payload: " + payload);
    Serial.println("http code: " + String(httpCode));
    Serial.println();

    http.end();
    return payload;
}

// ENVIANDO UM GET A UMA URL E RETORNANDO A RESPOSTA
String getWebData(String request, String URL){
    if(WiFi.status() != WL_CONNECTED)
        return "-1";
    
    HTTPClient http;
    http.begin(URL+request); 
    http.setTimeout(5000);

    int httpCode   = http.GET();
    String payload = (httpCode > 0) ? http.getString() : "-1";
    Serial.println("payload: " + payload);
    
    http.end();
    return payload;
}


// ENVIANDO UMA QUANDO FOR REQUISITADA UMA SOLICITAÇÃO HTTP
void sendServerResponse(String response){
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", response);
}

// FUNÇÃO QUE LIDA COM ATUALIZAÇÕES VIA OTA
void handleUpdate(){
    HTTPUpload& upload = server.upload();

    if(upload.status == UPLOAD_FILE_START){
        Serial.printf("Update: %s\n", upload.filename.c_str());

        if(!Update.begin(UPDATE_SIZE_UNKNOWN))
            Update.printError(Serial);
    }
    else if(upload.status == UPLOAD_FILE_WRITE){
        if(Update.write(upload.buf, upload.currentSize) != upload.currentSize)
            Update.printError(Serial);
    }
    else if(upload.status == UPLOAD_FILE_END){
        if(Update.end(true)){
            Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
            server.sendHeader("Access-Control-Allow-Origin",  "*");
            server.send(200, "text/plain",(Update.hasError())?"FAIL":"OK");
            ESP.restart();   
        }
        else{
            Update.printError(Serial);
        }
    }
}

// ENVIO DE INFORMAÇÕES POR HTTP GET
void onLogRequest(){
    StaticJsonDocument<600> info;
    info["event"] = "AAAA";
    
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", jsonToString(info));
}

// O ESP MANDA "OK" PARA INDICAR QUE ESTÁ FUNCIONANDO
void onCheckRequest(){
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "OK");
}

// REQUISIÇÃO PARA CONFIGURAR UMA VARIÁVEL PERMANENTE
void handleConfig(){
    if(!server.hasArg("plain")) 
        return;

    String request = server.arg("plain");
    StaticJsonDocument<200> data;  
    stringToJson(request, data);
    
    String key    = data["key"].as<String>();
    settings[key] = data["value"].as<String>();

    saveSettings();
    sendServerResponse("DONE");
}

// REQUISIÇÃO PARA OBTER UMA VARIÁVEL PERMANENTE DESTE MICROCONTROLADOR
void handleGetVariable(){
    if(!server.hasArg("plain")) 
        return;

    String request = server.arg("plain");
    String key     = request.substring(request.indexOf("$")+1, request.indexOf("!"));
    String value   = settings[key].as<String>();
    sendServerResponse(value);
}

void onAutoRequest(){
    if(!server.hasArg("plain")) 
        return;

    String request = server.arg("plain");
    sendServerResponse("OK");

    StaticJsonDocument<300> data;
    stringToJson(request, data);

    is_autonomous = true;
    redTime    = data["red"].as<int>();
    yellowTime = data["yellow"].as<int>();
    greenTime  = data["green"].as<int>();

    digitalWrite(RELAY_PIN, LOW);
    setBuzzer(BUZZER_GOOD_SOUND, 1000);
}

// REQUISIÇÃO GET PARA RESETAR O MICROCONTROLADOR
void onResetRequest(){
    sendServerResponse("OK");
    ESP.restart();
}

void onRelayHighRequest(){
    sendServerResponse("OK");
    digitalWrite(RELAY_PIN, HIGH);
    setBuzzer(BUZZER_GOOD_SOUND, 1000);
}

void onRelayLowRequest(){
    sendServerResponse("OK");
    digitalWrite(RELAY_PIN, HIGH);
    setBuzzer(BUZZER_GOOD_SOUND, 1000);
}


// FUNÇÃO QUE LIDA COM REQUISIÇÕES HTTP DE FORMA PERIÓDICA
void handleClient(){
    static unsigned long startTime = espMillis();
    
    if(espMillis() - startTime < 300)
        return;

    startTime = espMillis();
    server.handleClient();
}
