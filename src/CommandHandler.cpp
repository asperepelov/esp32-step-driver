#include "CommandHandler.h"

CommandHandler::CommandHandler(Tracker* tracker) : _tracker(tracker) {}

Result CommandHandler::handleCommand(const String& jsonString) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonString);

    if (error) {return Result(ResultCode::CommandError, "Ошибка разбора JSON");}
    if (!doc["command"].is<String>()) {return Result(ResultCode::CommandError, "Неверный формат команды");}        

    String command = doc["command"];

    if (command != "set_location"
        && command != "get_current_location"
        && command != "get_current_azimuth"
        && command != "get_info") 
    {
        if (!doc["value"].is<int>()) {return Result(ResultCode::CommandError, "Неверный формат команды");}        
    }

    if (command == "turn_azimuth_angle") {
        return _tracker->turnAzimuthAngle(doc["value"]);
    }
    else if (command == "turn_to_azimuth") {
        return _tracker->turnToAzimuth(doc["value"]);
    }
    else if (command == "set_azimuth") {
        return _tracker->setCurrentAzimuth(doc["value"]);
    }
    else if (command == "set_location") {
        if (!doc["value"]["lat"].is<double>() || !doc["value"]["lon"].is<double>()) {
            return Result(ResultCode::CommandError, "Неверный формат команды");
        }
        return _tracker->setCurrentLocation(Location(doc["value"]["lat"], doc["value"]["lon"]));
    }    
    else if (command == "set_online") {
        int value = doc["value"];
        if (value == 1) {
            return _tracker->online();
        } else if (value == 0) {
            return _tracker->offline();
        } else {
            return Result(ResultCode::CommandError, "Недопустимое значение для set_online");
        }
    }
    else if (command == "enable_ins") {
        int value = doc["value"];
        if (value == 1) {
            return _tracker->enableINS();
        } else if (value == 0) {
            return _tracker->disableINS();
        } else {
            return Result(ResultCode::CommandError, "Недопустимое значение для set_online");
        }
    }    
    else if (command == "get_current_azimuth") {
        return Result(ResultCode::Success, String(_tracker->getCurrentAzimuth()));
    }
    else if (command == "get_current_location") {
        return Result(ResultCode::Success, (*_tracker->getCurrentLocation()));
    }
    else if (command == "get_info") {
        char str[256];
        snprintf(str, sizeof(str), "{azimuth:%d, position:%s}", 
            _tracker->getCurrentAzimuth(),
            String(_tracker->getCurrentLocation()->operator String()).c_str());
        return Result(ResultCode::Success, str);
    }
    else {
        return Result(ResultCode::UnknownCommand, "Неизвестная команда: " + command);
    }
}