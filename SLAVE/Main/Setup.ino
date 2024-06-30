// CONECTANDO À REDE LOCAL PELO NOME E SENHA
void connectWiFi(String wifi_ssid, String wifi_password){
    WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str(), 1);

    const unsigned long startTime = espMillis();
    Serial.println();

    while(WiFi.status() != WL_CONNECTED)
        if(espMillis() - startTime > 2000)
            break;

    if (WiFi.status() == WL_CONNECTED)
        Serial.println("WIFI CONECTADO!");
    else
        Serial.println("WIFI NÃO CONECTADO!");

    Serial.print("ENDEREÇO IP: ");
    Serial.println(WiFi.localIP());
}

void startServer(String ssid, String pass){ 
    IPAddress staticIP(192, 168, 1, 2);   // IP ESTÁTICO
    IPAddress gateway(192, 168, 1, 2);    // GATEWAY ESTÁTICO IP
    IPAddress subnet(255, 255, 255, 0);

    WiFi.softAPConfig(staticIP, gateway, subnet);
    delay(100);

    WiFi.setTxPower(WIFI_POWER_19_5dBm);
    WiFi.softAP(ssid.c_str(), pass.c_str(), 1);

    Serial.print("SERVER STARTED: ");
    Serial.println(WiFi.softAPIP());
}

void routesSetup(){
    server.on("/update", HTTP_POST, handleUpdate);
    server.on("/config", HTTP_POST, handleConfig);
    server.on("/getVar", HTTP_POST, handleGetVariable);
    server.on("/INFO",   HTTP_GET,  onLogRequest);
    server.on("/CHECK",  HTTP_GET,  onCheckRequest);
    server.on("/RESET",  HTTP_GET,  onResetRequest);
}


// INICIA UM CAMPO DE ARMAZENAMENTO CHAMADO "preferences" E INICIA ELE COM VAZIO "{}"
void storageSetup(){
    preferences.begin("preferences", false);
    String jsonString = preferences.getString("settings", "{}");
    preferences.end();

    if (jsonString == "{}")
        return getStandardConfigs();
    
    stringToJson(jsonString, settings);
}

// INICIA AS ROTAS DO SERVER MASTER
void wifiSetup(){
    String ssid = settings["ssid"].as<String>();
    String pass = settings["passwd"].as<String>();
        
    WiFi.mode(WIFI_STA);
    connectWiFi(ssid, pass);
    startServer("ESP_ID", "12345678");
    
    routesSetup();
    server.begin();
}

// CONFIGURANDO A MEMÓRIA DO ESP PARA ARMAZENAR ARQUIVOS DE TEXTO
void spiffsSetup() {
    if (LittleFS.begin()) {
        Serial.println("LittleFS montado com sucesso!");
        return;
    }
    
    Serial.println("Erro ao montar LittleFS. Tentando formatar...");

    if (!LittleFS.format()) {
        Serial.println("Falha ao formatar LittleFS!");
        return;
    }

    Serial.println("LittleFS formatado! Agora tentando inicializar novamente...");

    if (!LittleFS.begin()) {
        Serial.println("Falha ao montar LittleFS após a formatação!");
        return;
    }

    Serial.println("LittleFS montado após formatação!");
}

