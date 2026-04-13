//
// Created by Harry Skerritt on 13/04/2026.
//

#include "SerialManager.h"


void SerialManager::update() {
    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();

        if (cmd == START_GET) {
            sendFile();
        } else if (cmd == START_SAVE) {
            receiveFile();
        }
    }
}

void SerialManager::sendFile() {
    if (!LittleFS.exists("/config.toml")) {
        Serial.println("ERROR: Config file not found");
        Serial.println(END_MARKER);
        return;
    }

    File file = LittleFS.open("/config.toml", "r");
    while (file.available()) {
        Serial.write(file.read());
    }
    file.close();
    Serial.println("\n" + END_MARKER);
}

void SerialManager::receiveFile() {
    File file = LittleFS.open("/config.toml", "w");
    if (!file) return;

    while (true) {
        if (Serial.available()) {
            String line = Serial.readStringUntil('\n');
            line.trim();

            if (line == END_MARKER) break;

            file.println(line);
        }
    }
    file.close();

    Serial.println("SUCCESS");
    delay(500);
}
