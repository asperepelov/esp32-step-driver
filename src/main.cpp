/* Настройки для управления шаговым двигателем трекера Chinowing Tracking System V31
управление через Microstep Driver TB6600
Microstep = 4, 90град = 1080 импульсов, 1град = 12 импульсов
*/

#include <Arduino.h>
#include "Tracker.h"
#include "CommandHandler.h"
#include "EthernetManager.h"
#include <stdlib.h>

//================================================================
// Настройки трекера
#define DIR_PIN 2 // GPIO12 (D12) подключен к DIR+ на TB6600
#define STEP_PIN 4 // GPIO13 (D13) подключен к PUL+ на TB6600
#define ENA_PIN 12 // GPIO14 (D14) подключен к ENA+ на TB6600

StepMotor motor(STEP_PIN, DIR_PIN, ENA_PIN, 500, 2000);
Tracker tracker(&motor, 12, 10, false, TrackerState::Online);
CommandHandler commandHandler(&tracker);
//================================================================
// Настройки сети
IPAddress local_IP(192, 168, 144, 54);
IPAddress gateway(192, 168, 144, 1);
IPAddress netMask(255, 255, 255, 0);
EthernetManager ethernetManager(local_IP, gateway, netMask);
//================================================================
// TCP сервер
#define TCP_PORT 80
WiFiServer tcpServer(TCP_PORT);
//================================================================

// Упаковка ответа в JSON
String packResultToJson(const Result& res);


void setup() {
  Serial.begin(115200);
  Serial.println("СТАРТ");

    Serial.println("Инициализация Ethernet...");
    if (ethernetManager.begin()) {
        // wifiManager.setup();
        tcpServer.begin();
        Serial.println("TCP сервер запущен");
    } else {
        Serial.println("Не удалось запустить Ethernet. Перезагрузка...");
        ESP.restart();
    }

  // Активируем двигатель трекера
  tracker.online();
  Serial.println("Трекер переведён в онлайн");
}

void loop() {
  WiFiClient client = tcpServer.available();

  if (client) {
    Serial.println("Новый клиент подключен");
    
    while (client.connected()) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        line.trim();
        
        Result res = commandHandler.handleCommand(line);
        String jsonRes = packResultToJson(res);
        if (res.code == ResultCode::Success) {
          if (res.message != "") {client.println(jsonRes);}
          else {client.println("Выполнено");}
        } else {
          client.println(jsonRes);
        }
      }
    }
    
    client.stop();
    Serial.println("Клиент отключен");
  }

  delay(1);
}


// Упаковка ответа в JSON
String packResultToJson(const Result& res) {
    StaticJsonDocument<200> doc;
    
    doc["code"] = static_cast<int>(res.code);    
    if (res.code == ResultCode::Success) {
        doc["status"] = "success";
    } else {
        doc["status"] = "error";
    }
    doc["message"] = res.message;

    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}