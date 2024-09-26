/* Настройки для управления шаговым двигателем трекера Chinowing Tracking System V31
управление через Microstep Driver TB6600
Microstep = 4, 90град = 1080 импульсов, 1град = 12 импульсов
*/

#include <Arduino.h>
#include "Tracker.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include "WiFiManager.h"
#include "EthernetManager.h"
#include <ETH.h>

#define STEP_PIN 12  // GPIO13 (D13) подключен к PUL+ на TB6600
#define DIR_PIN 13   // GPIO12 (D12) подключен к DIR+ на TB6600
#define ENA_PIN 14   // GPIO14 (D14) подключен к ENA+ на TB6600

StepMotor motor(STEP_PIN, DIR_PIN, ENA_PIN, 2000);
Tracker tracker(&motor, 12, false);

const IPAddress netMask = IPAddress(255, 255, 255, 0);
#define TCP_PORT 5600
WiFiServer server(TCP_PORT);
ClientManager clientManager;
WiFiManager wifiManager("fix_tr", "00000000",
                        IPAddress(192, 168, 1, 21),
                        netMask
);
// EthernetManager ethernetManager(
//     IPAddress(192, 168, 144, 55),
//     IPAddress(192, 168, 144, 1),
//     netMask
// );

#define BUFFER_SIZE 1024 * 4
static uint8_t buffer[BUFFER_SIZE];


void setup() {
  Serial.begin(115200);
  Serial.println("СТАРТ");
  delay(1000);

  Serial.println("Инициализация Ethernet...");
  // if (ethernetManager.begin()) {
      wifiManager.setup();
      // dataProcessor.begin();
      server.begin();
      Serial.printf("TCP сервер запущен на порту :%d\n", TCP_PORT);
  // } else {
  //     Serial.println("Не удалось запустить Ethernet. Перезагрузка...");
  //     ESP.restart();
  // }
}

void loop() {
  // ethernetManager.checkAndRestartEthernet();
  // if (!ethernetManager.isConnected()) {
  //     delay(1000);
  //     return;
  // }
  if (server.hasClient()) {
      WiFiClient client = server.available();
      clientManager.handleNewConnection(client);
  }

  clientManager.updateClients();

  // Обработка данных от клиентов
  for (int i = 0; i < MAX_CLIENTS; i++) {
      if (clientManager.isClientConnected(i)) {
          WiFiClient &client = clientManager.getClient(i);
          int bytesAvailable = client.available();
          if (bytesAvailable > 0) {
              // Чтение данных от клиента
              int bytesToRead = min(bytesAvailable, BUFFER_SIZE);
              int bytesRead = client.read(buffer, bytesToRead);

              Serial.printf("Сообщение: %s\n", buffer);
          }
      }
  }

  // Небольшая задержка для стабильности работы
  delay(1);


  // int angle = 5;

  // Serial.println("Перевод в онлайн");
  // tracker.online();

  // Serial.println("Поворот на " + String(angle));
  // tracker.turnAzimut(angle);
  
  // Serial.println("Ожидание...");
  // delay(5000); 
  
  // Serial.println("Поворот на " + String(-angle));
  // tracker.turnAzimut(-angle);
  
  // Serial.println("Перевод в оффлайн");
  // tracker.offline();
  
  // Serial.println("Ожидание...");
  // delay(5000); 
}