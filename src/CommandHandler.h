#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <ArduinoJson.h>
#include "Tracker.h"

class CommandHandler {
private:
    Tracker* _tracker; // Трекер    
public:
    CommandHandler(Tracker* tracker);
    // Обработать команду
    Result handleCommand(const String& jsonString);
};

#endif // COMMAND_HANDLER_H