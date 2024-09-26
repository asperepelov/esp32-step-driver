/* Настройки для управления шаговым двигателем трекера Chinowing Tracking System V31
управление через Microstep Driver TB6600
Microstep = 4, 90град = 1080 импульсов, 1град = 12 импульсов
*/

#include <Arduino.h>
#include "Tracker.h"
#include "CommandHandler.h"
#include <WiFi.h>
#include <stdlib.h>

#define STEP_PIN 12  // GPIO13 (D13) подключен к PUL+ на TB6600
#define DIR_PIN 13   // GPIO12 (D12) подключен к DIR+ на TB6600
#define ENA_PIN 14   // GPIO14 (D14) подключен к ENA+ на TB6600

StepMotor motor(STEP_PIN, DIR_PIN, ENA_PIN, 2000);
Tracker tracker(&motor, 12, false);
CommandHandler commandHandler(&tracker);

#define SSID "fix_tr"
#define PSWD "24000000"

// Задаем статический IP-адрес
IPAddress local_IP(192, 168, 144, 54);
IPAddress gateway(192, 168, 144, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(80);


void setup() {
  Serial.begin(115200);
  Serial.println("СТАРТ");

  // Настраиваем статический IP-адрес
  if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
    Serial.println("Ошибка конфигурации статического IP");
  }

  // Создаем Wi-Fi точку доступа
  WiFi.softAP(SSID, PSWD);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("IP-адрес точки доступа: ");
  Serial.println(IP);

  // Запускаем TCP сервер
  server.begin();
  Serial.println("Сервер запущен");  

  // Активируем двигатель трекера
  tracker.online();
  Serial.println("Трекер переведён в онлайн");

}

bool isNumber(const String& str) {
    char* endptr = nullptr;
    strtol(str.c_str(), &endptr, 10);
    return *endptr == '\0';
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("Новый клиент подключен");
    
    while (client.connected()) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        line.trim();
        // commandHandler.
        
        // Отправляем ответ клиенту (необязательно)
        // client.println("Сообщение получено");
      }
    }
    
    client.stop();
    Serial.println("Клиент отключен");
  }

  delay(1);
}