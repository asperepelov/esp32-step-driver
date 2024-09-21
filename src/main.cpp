#include <Arduino.h>

#define STEP_PIN 12  // GPIO13 (D13) подключен к PUL+ на TB6600
#define DIR_PIN 13   // GPIO12 (D12) подключен к DIR+ на TB6600
#define ENA_PIN 14   // GPIO14 (D14) подключен к ENA+ на TB6600
#define PULSE_CNT 200 // Кол-во импульсов

// Прототип функции
void step();

// Направление поворота
enum class TurnDirection {Plus = LOW, Minus = HIGH};

// Перевод структуры направления вращения в строку
String getTurnDirectionString(TurnDirection direction) {
  switch (direction) {
    case TurnDirection::Plus:
      return "по часовой стрелке";
    case TurnDirection::Minus:
      return "против часовой стрелки";
    default:
      return "в неизвестном направлении";
  }
}

// Вращение мотора в заданном направлении
void turnMotor(int pulseCnt, TurnDirection dir) {
  Serial.println("Вращение " + getTurnDirectionString(dir));
  digitalWrite(DIR_PIN, uint8_t(dir));
  for(int i = 0; i < pulseCnt; i++) {
    step();
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENA_PIN, OUTPUT);
  
  digitalWrite(ENA_PIN, LOW); // Включение драйвера двигателя
}

void loop() {
  int pulseCnt = PULSE_CNT;

  turnMotor(pulseCnt, TurnDirection::Plus);
  
  Serial.println("Ожидание 3 секунды");
  delay(3000); 
  
  turnMotor(pulseCnt, TurnDirection::Minus);
  
  Serial.println("Ожидание 3 секунды");
  delay(3000); 
}

void step() {
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(500);
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(500);
}