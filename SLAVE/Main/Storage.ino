

// SALVA NA MEMÓRIA DO ESP AS INFORMAÇÕES DE SETTINGS ATUALIZADAS
void saveSettings(){
    settings["alive_time"] = getSeconds(espMillis());

    preferences.begin("preferences", false);
    preferences.putString("settings", jsonToString(settings));
    preferences.end();
}

// APAGANDO TODO O CONTEÚDO DO JSON DE CONFIGURAÇÕES
void eraseSettings(){
    preferences.begin("preferences", false);
    preferences.putString("settings", "{}");
    preferences.end();
}

// IMPORTANDO AS CONFIGURAÇÕES INICIAIS DE INICIALIZAÇÃO
void getStandardConfigs(){
    settings["alive_time"] = espMillis();
    settings["ssid"]   = "Kakau";
    settings["passwd"] = "kakau123";
    settings["esp_id"] = "MIC-MASTER-06425679";
    settings["server_enabled"] = true;
    saveSettings();
}

void writeFile(const char* path, const String& message) {
    File file = LittleFS.open(path, FILE_WRITE);
    
    if(!file) {
        Serial.println("Erro ao abrir o arquivo para escrita");
        return;
    }

    if(!file.print(message))
        Serial.println("Erro ao escrever no arquivo");

    file.close();
}

String readFile(const char* path){
    File file = LittleFS.open(path, FILE_READ);

    if(!file){
        Serial.println("Erro ao abrir o arquivo para leitura");
        return String();  // Retorna uma String vazia
    }

    String content = file.readString();
    file.close();
    return content;
}

void appendFile(const char* path, const String& message) {
    File file = LittleFS.open(path, FILE_APPEND);

    if(!file){
        Serial.println("Erro ao abrir o arquivo para adição");
        return;
    }

    if(!file.print(message))
        Serial.println("ERRO AO SALVAR TEXTO NO SPIFFS");
    
    file.close();
}

int getFileLength(const char* path){
    File file = LittleFS.open(path, FILE_READ);
    
    if (!file)
        return -1;

    int size = file.size();
    file.close();
    return size;
}

String readFileChunk(const char* path, int length) {
    File file = LittleFS.open(path, FILE_READ);
    String content = "";

    if (!file) {
        Serial.println("Erro ao abrir o arquivo para leitura");
        return "";
    }

    int fileSize = file.size();
    length = min(length, (int)fileSize);    // Garante que length não seja maior que o tamanho do arquivo

    if (length > 0) {
        char buffer[length + 1];            // Cria um buffer com tamanho length + 1 para o caractere nulo
        file.readBytes(buffer, length);
        buffer[length] = '\0';              // Adiciona o caractere nulo ao final do buffer
        content = String(buffer);
    } 
    else {
        content = file.readString();
    }

    file.close();
    return content;
}

void shiftFile(const char* path, int n) {
    const size_t bufferSize = 512;  // Tamanho do buffer para ler e escrever em partes
    char buffer[bufferSize];

    // Abre o arquivo original para leitura
    File file = LittleFS.open(path, FILE_READ);
    if (!file) {
        Serial.println("Erro ao abrir o arquivo para leitura");
        return;
    }

    // Cria um arquivo temporário para escrever o conteúdo atualizado
    File tempFile = LittleFS.open("/temp.txt", FILE_WRITE);
    if (!tempFile) {
        Serial.println("Erro ao criar o arquivo temporário");
        file.close();
        return;
    }

    // Pula os primeiros n caracteres
    int skipped = 0;
    while (skipped < n && file.available()) {
        int toSkip = min(n - skipped, (int) bufferSize);
        file.readBytes(buffer, toSkip);
        skipped += toSkip;
    }

    // Lê o restante do arquivo e escreve no arquivo temporário
    while (file.available()) {
        size_t bytesRead = file.readBytes(buffer, bufferSize);
        tempFile.write((const uint8_t*)buffer, bytesRead);
    }

    file.close();
    tempFile.close();

    // Remove o arquivo original
    LittleFS.remove(path);

    // Renomeia o arquivo temporário para o nome original
    LittleFS.rename("/temp.txt", path);
}
