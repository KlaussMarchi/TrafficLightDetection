// RECEBE UM OBJETO JSON E CONVERTE EM UMA STRING FORMATADA
String jsonToString(const JsonDocument& jsonObj){
    String jsonString;
    serializeJson(jsonObj, jsonString); 
    return jsonString;
}

// INSERE EM OBJETO JSON UMA STRING DOS DADOS A SEREM CONVERTIDOS 
bool stringToJson(String &jsonString, JsonDocument &doc) {
    DeserializationError error = deserializeJson(doc, jsonString);

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return false;
    }

    return true;
}

// DELAY ASSÍNCRONO COM FUNÇÕES RODANDO EM SEGUNDO PLANO
void asyncDelay(int ms){
    unsigned long startTime = espMillis();

    while(espMillis() - startTime < ms)
        checkBgTasks();
}

// DELAY ASSÍNCRONO COM AS REQUISIÇÕES WEB AINDA RODANDO
void serverDelay(int ms){
    unsigned long startTime = espMillis();
    
    while(espMillis() - startTime < ms)
        handleClient();
}

// RETORNA O TEMPO EM MILISSEGUNDOS DESDE QUE O DISPOSITIVO FOI LIGAOD
unsigned long espMillis(){
    return esp_timer_get_time()/1000;
}

// CONVERTE MILISSEGUNDOS PARA SEGUNDOS EM UMA VARIÁVEL UNSIGNED LONG
float getSeconds(unsigned long ms){
    return ms/1000.0;
}

void setBuzzer(int frequency, int delayTime){
    ledcWriteTone(0, frequency);

    if(frequency != BUZZER_NO_SOUND)
        ledcWrite(0, volume);
    
    if(delayTime == 0)
        return;
    
    serverDelay(delayTime);
    ledcWriteTone(0, 0);
}