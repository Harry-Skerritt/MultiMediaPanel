//
// Created by Harry Skerritt on 13/04/2026.
//

#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#include <Arduino.h>
#include <LittleFS.h>
#include <FS.h>


class SerialManager {
public:
    void update();

private:
    void sendFile();
    void receiveFile();

    const String START_GET = "GET_CONFIG";
    const String START_SAVE = "SAVE_CONFIG";
    const String END_MARKER = "END_MARKER";
};


#endif //SERIALMANAGER_H
