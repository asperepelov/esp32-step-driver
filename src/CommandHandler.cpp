#include "CommandHandler.h"

CommandHandler::CommandHandler(Tracker* tracker) : _tracker(tracker) {}

Result CommandHandler::handleCommand(const String& jsonString) {
    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, jsonString);

    if (error) {
        return Result(ResultCode::CommandError, "Ошибка разбора JSON");
    }

    if (!doc.containsKey("command") || !doc.containsKey("value")) {
        return Result(ResultCode::CommandError, "Неверный формат команды");
    }

    String command = doc["command"];
    int value = doc["value"];

    if (command == "turn_azimuth_angle") {
        return _tracker->turnAzimuthAngle(value);
    }
    else if (command == "turn_to_azimuth") {
        return _tracker->turnToAzimuth(value);
    }
    else if (command == "set_azimuth") {
        return _tracker->setCurrentAzimuth(value);
    }
    else if (command == "set_online") {
        if (value == 1) {
            return _tracker->online();
        } else if (value == 0) {
            return _tracker->offline();
        } else {
            return Result(ResultCode::CommandError, "Недопустимое значение для set_online");
        }
    }
    else if (command == "get_current_azimuth") {
            return Result(ResultCode::Success, String(_tracker->getCurrentAzimuth()));
    }
    else {
        return Result(
            ResultCode::UnknownCommand, 
            "Неизвестная команда: " + command
            );
        
    }
}