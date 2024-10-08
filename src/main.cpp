/* Настройки для управления шаговым двигателем трекера Chinowing Tracking System V31
управление через Microstep Driver TB6600
Microstep = 4, 90град = 1080 импульсов, 1град = 12 импульсов
*/

#include <Arduino.h>
#include "Tracker.h"
#include "CommandHandler.h"
#include "EthernetManager.h"
#include <stdlib.h>
#include <mavlink.h>
#include "esp_task_wdt.h"

//================================================================
// Настройки трекера
#define DIR_PIN 2 // GPIO12 (D12) подключен к DIR+ на TB6600
#define STEP_PIN 4 // GPIO13 (D13) подключен к PUL+ на TB6600
#define ENA_PIN 12 // GPIO14 (D14) подключен к ENA+ на TB6600

StepMotor motor(STEP_PIN, DIR_PIN, ENA_PIN, 500, 2000);
Tracker tracker(&motor, 12, 30, false, TrackerState::Online);
CommandHandler commandHandler(&tracker);
//================================================================
// Настройки сети
IPAddress local_IP(192, 168, 144, 54);
IPAddress gateway(192, 168, 144, 1);
IPAddress netMask(255, 255, 255, 0);
EthernetManager ethernetManager(local_IP, gateway, netMask);
//================================================================
// TCP сервер
#define TCP_COMMAND_PORT 80
WiFiServer tcpCommandServer(TCP_COMMAND_PORT);
//================================================================
// TCP клиент для MAVLink
#define MAVLINK_SERVER_IP "192.168.144.53"
#define MAVLINK_SERVER_PORT 5600
WiFiClient mavlinkClient;
//================================================================
// Задачи FreeRTOS
TaskHandle_t taskTcpCommand;
TaskHandle_t taskMavlink;


// Функция для расчета угла поворота (замените на вашу реальную функцию)
int16_t calculateTurnAngle(int32_t lat, int32_t lon, int32_t alt) {
    // Здесь должна быть ваша логика расчета угла поворота
    Serial.println("Расчет угла поворота");
    return 0; // Заглушка
}

// Упаковка ответа в JSON
String packResultToJson(const Result& res);

// Обработка команд для трекера
void taskTcpCommandCode(void * parameter) {
  for(;;) {
    try {
      WiFiClient client = tcpCommandServer.available();

      if (client) {
        Serial.println("Новый клиент подключен");
        // client.println(packResultToJson(Result(ResultCode::Connected, "Соединение установлено")));
        
        while (client.connected()) {
          if (client.available()) {
            String line = client.readStringUntil('\n');
            line.trim();
            
            Result res = commandHandler.handleCommand(line);
            String jsonRes = packResultToJson(res);
            client.println(jsonRes);
          }
        }
        
        client.stop();
        Serial.println("Клиент отключен");
      }   
    } catch (const std::exception& e) {
        Serial.println("Ошибка в taskTcpCommandCode: " + String(e.what()));
        // Логирование ошибки, попытка восстановления и т.д.
    }

    vTaskDelay(1 / portTICK_PERIOD_MS); 
  }
}

// Обработка телеметрии
void taskMavlinkCode(void * parameter) {
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(3000); // 3 секунды

    xLastWakeTime = xTaskGetTickCount();

    for(;;) {
        try {
            if (!tracker.isEnableINS()) {
                Serial.println("Работа по ИНС отключена");
                vTaskDelay(10000 / portTICK_PERIOD_MS);
                continue;                
            }
            if (!mavlinkClient.connected()) {
                if (mavlinkClient.connect(MAVLINK_SERVER_IP, MAVLINK_SERVER_PORT)) {
                    Serial.println("Подключено к MAVLink серверу");
                } else {
                    Serial.println("Не удалось подключиться к MAVLink серверу");
                    vTaskDelay(5000 / portTICK_PERIOD_MS);
                    continue;
                }
            }

            mavlink_message_t msg;
            mavlink_status_t status;
            bool messageReceived = false;

            // Читаем данные в течение короткого промежутка времени
            unsigned long startTime = millis();
            while (millis() - startTime < 100) { // Читаем в течение 100 мс
                if (mavlinkClient.available()) {
                    uint8_t c = mavlinkClient.read();
                    if (mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {
                        messageReceived = true;
                        break;
                    }
                }
            }

            if (messageReceived && msg.msgid == MAVLINK_MSG_ID_GLOBAL_POSITION_INT) {
                mavlink_global_position_int_t pos;
                mavlink_msg_global_position_int_decode(&msg, &pos);
                Serial.printf("Координаты объекта: %d, %d\n", pos.lat, pos.lon);
                RadialLocation radLoc = tracker.calcRadialLocation(pos.lat/10000000.0, pos.lon/10000000.0);
                Serial.printf("Радиальное положение: дистанция = %.0fм, азимут = %f\n", radLoc.distance, radLoc.azimuth);
                int16_t azimuth = radLoc.azimuth;
                mavlinkClient.stop();

                Result res = tracker.turnToAzimuth(azimuth);
                Serial.println(packResultToJson(res));
            }

        } catch (const std::exception& e) {
            Serial.println("Ошибка в taskMavlinkCode: " + String(e.what()));
            // Логирование ошибки, попытка восстановления и т.д.
        }  

        // Ждем до следующего 3-секундного интервала
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void setup() {
  Serial.begin(115200);
  Serial.println("СТАРТ");

  delay(10000);
  Serial.println("Инициализация Ethernet...");
  if (ethernetManager.begin()) {
      // wifiManager.setup();
      tcpCommandServer.begin();
      Serial.println("TCP сервер запущен");
  } else {
      Serial.println("Не удалось запустить Ethernet. Перезагрузка...");
      ESP.restart();
  }

  // Активируем двигатель трекера
  tracker.online();
  Serial.println("Трекер переведён в онлайн");

  esp_task_wdt_init(10, true); // WDT тайм-аут, чтобы трекер успел повернуться на 360град
  xTaskCreatePinnedToCore(taskTcpCommandCode, "TCPCommand", 4096, NULL, 1, &taskTcpCommand, 0);
  xTaskCreatePinnedToCore(taskMavlinkCode, "MAVLink", 4096, NULL, 1, &taskMavlink, 1);
}

void loop() {
  // Выполнение 
  // taskTcpCommandCode
  // taskMavlinkCode
  vTaskDelay(1000 / portTICK_PERIOD_MS);
} 


// Упаковка ответа в JSON
String packResultToJson(const Result& res) {
    JsonDocument doc;
    
    doc["code"] = static_cast<int>(res.code);    
    if (res.code == ResultCode::Success) {
        doc["status"] = "success";
    } else if (res.code == ResultCode::Connected) {
        doc["status"] = "connected";
    } else {
        doc["status"] = "error";
    }
    doc["message"] = res.message;

    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}