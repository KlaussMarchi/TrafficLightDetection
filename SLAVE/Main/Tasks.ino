// OPERAÇÕES QUE FICAM SEMPRE RODANDO NO PROGRAMA
void checkBgTasks(){
    handleServer();
    handleSerialListen();
    handleClient();
}

// VERIFICAÇÃO DE INPUTS VIA COMUNICAÇÃO SERIAL
void handleSerialListen(){
    if(!Serial.available())
      return;
    
    String response = Serial.readString();
    response.trim();
    Serial.println("recieved: " + response);

    if(response.indexOf("CF") != -1)
      handleConfigRecieved(response);

    if(response.indexOf("ID") != -1)
      handleInfoRecieved(response);

    if(response.indexOf("CMD") != -1)
      handleCommandRecieved(response);
}

// PROCESSANDO COMANDO DE CONFIGURAÇÃO
void handleConfigRecieved(String command){
    String key   = command.substring(command.indexOf("$")+1, command.indexOf(":"));
    String value = command.substring(command.indexOf(":")+1, command.indexOf("!"));

    settings[key] = value;
    saveSettings();

    value = settings[key].as<String>();
    Serial.println("configured!");
    
    Serial.println("key: " + key + " | value: " + value);
    Serial.println();
}

// PROCESSANDO COMANDO DE IDENTIFICAÇÃO
void handleInfoRecieved(String command){
    String key   = command.substring(command.indexOf("$")+1, command.indexOf("!"));
    String value = settings[key].as<String>();

    Serial.println("key: " + key + " | value: " + value);
    Serial.println();
}

// PROCESSANDO UM COMANDO DE FUNÇÃO RECEBIDO
void handleCommandRecieved(String command){
    String key = command.substring(command.indexOf("$")+1, command.indexOf("!"));
    Serial.println("cmd: " + key);

    if(key == "send_server")
      return getServerInfo();
}

// COMUNICAÇÃO IoT COM O BANCO DE DADOS DO SERVIDOR
void getServerInfo(){
    StaticJsonDocument<600> data;
    data["esp_id"]  = settings["esp_id"].as<String>();
    data["command"] = "example data, just debugging";

    Serial.println("enviando requisição");
    String response = postWebData(serverURL, "logs/", jsonToString(data));
    //String response = getWebData(serverURL + "logs/");

    if(response == "-1"){
      Serial.println("SEM RESPOSTA");
      return;
    }
    
    stringToJson(response, data);

    if(data["status"].as<String>() == "error"){
      Serial.println(data["status"].as<String>());
      return;
    }

    int PH  = data["data"]["PH"].as<int>();
    int tmp = data["data"]["temperature"].as<int>(); 

    Serial.println("PH: " + String(PH));
    Serial.println("TEMPERATURA: " + String(tmp)); 
}

// LIDANDO DE MANEIRA ASSÍNCRONA COM O SERVIDOR
void handleServer(){
    static unsigned long startTime = espMillis();
    
    if(espMillis() - startTime < 60000)
      return;

    startTime = espMillis();
    getServerInfo();
}