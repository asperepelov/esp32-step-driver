#ifndef RESULT_H
#define RESULT_H

#include <Arduino.h>

// Коды результатов операций трекера
enum class ResultCode {
    Success = 0,
    Connected,
    ParamOutOfRange,
    TrackerIsOffline,
    UnknownCommand,
    CommandError
};

// Структура для возврата результата операции
struct Result {
    ResultCode code;
    String message;

    Result(ResultCode c, const String& msg) 
        : code(c), message(msg) {}
};

#endif // RESULT_H