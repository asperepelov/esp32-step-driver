#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <ArduinoJson.h>
#include "Tracker.h"

class CommandHandler {
public:
    CommandHandler(Tracker* tracker) : _tracker(tracker) {}

    void handleCommand(const String& jsonString) {
        DynamicJsonDocument doc(256);
        DeserializationError error = deserializeJson(doc, jsonString);

        if (error) {
            Serial.println("Ошибка разбора JSON");
            return;
        }

        if (!doc.containsKey("command") || !doc.containsKey("value")) {
            Serial.println("Неверный формат команды");
            return;
        }

        String command = doc["command"];
        int value = doc["value"];

        if (command == "turn_azimut_angle") {
            _tracker->turnAzimut(value);
            Serial.println("Поворот на угол: " + String(value));
        }
        else if (command == "turn_to_azimut") {
            _tracker->turnToAzimut(value);
            Serial.println("Поворот к азимуту: " + String(value));
        }
        else if (command == "set_azimut") {
            _tracker->setAzimut(value);
            Serial.println("Установка азимута: " + String(value));
        }
        else if (command == "set_online") {
            if (value == 1) {
                _tracker->online();
                Serial.println("Трекер переведен в онлайн");
            } else if (value == 0) {
                _tracker->offline();
                Serial.println("Трекер переведен в офлайн");
            } else {
                Serial.println("Недопустимое значение для set_online");
            }
        }
        else {
            Serial.println("Неизвестная команда: " + command);
        }
    }

private:
    Tracker* _tracker;
};

#endif // COMMAND_HANDLER_H