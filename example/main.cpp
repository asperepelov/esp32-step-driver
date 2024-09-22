/* Настройки для управления шаговым двигателем трекера
управление через Microstep Driver TB6600
Microstep = 4, 90град = 1080 импульсов, 1град = 12 импульсов
*/

#include <Arduino.h>
#include "Tracker.h"

#define STEP_PIN 12  // GPIO13 (D13) подключен к PUL+ на TB6600
#define DIR_PIN 13   // GPIO12 (D12) подключен к DIR+ на TB6600
#define ENA_PIN 14   // GPIO14 (D14) подключен к ENA+ на TB6600

StepMotor *motor;
Tracker *tracker;

void setup() {
  Serial.begin(115200);

  motor = new StepMotor(STEP_PIN, DIR_PIN, ENA_PIN, 2000);
  tracker = new Tracker(motor, 12, false);
}

void loop() {
  int angle = 5;

  Serial.println("Перевод в онлайн");
  tracker->online();

  Serial.println("Поворот на " + String(angle));
  tracker->turnAzimut(angle);
  
  Serial.println("Ожидание...");
  delay(5000); 
  
  Serial.println("Поворот на " + String(-angle));
  tracker->turnAzimut(-angle);
  
  Serial.println("Перевод в оффлайн");
  tracker->offline();
  
  Serial.println("Ожидание...");
  delay(5000); 
}