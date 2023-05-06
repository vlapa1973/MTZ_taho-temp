#include <Arduino.h>
/*
    Динамическая индикация
    202004-04
    v1.0
*/

#include <OneWire.h>
OneWire ds(12);

// список выводов Arduino для подключения к сегментам a-g
// семисегментного индикатора
uint8_t pinsCat[] = {9, 13, 4, 6, 7, 10, 3, 5};

// список выводов Arduino для подключения к разрядам 1-4
// семисегментного индикатора
uint8_t pinsAn[] = {14, 15, 16, 17};

// значения для вывода цифр 0-9
byte numbers[] = { B11111100, B01100000, B11011010,
                   B11110010, B01100110, B10110110,
                   B10111110, B11100000, B11111110,
                   B11110110, B11000110, B00000000
                 };

// переменная для хранения значения текущей цифры
byte number[] = {0, 0, 0, 0};

// переменная для хранения текущего разряда
byte digit = 0;

uint32_t oldMillis = 0;
byte bright = 1;
uint16_t timer = 1000;

byte data[2];
uint16_t randFig = 0;

byte button = 11;
bool buttonState = false;
bool flag = false;

volatile uint32_t timeI = 0;
volatile uint32_t time_last = 0;

const uint16_t timeCount = 64;
uint16_t timeCountTemp = 0;
float timeTemp = 0;

//===========================================================
// функция вывода цифры на семисегментный индикатор
// данные:  number[0-3]
void figure() {
  if (randFig / 1000) {
    number[0] = randFig / 1000;
  } else {
    number[0] = 11;
  }
  number[1] = randFig % 1000 / 100;
  number[2] = randFig % 100 / 10;
  //  number[3] = randFig % 10;
  if (buttonState) {
    number[3] = 10;
  } else {
    number[3] = 0;
  }
  for (uint8_t n = 0; n < 4; n++) {
    if (number[n] > 11) {
      number[n] = 11;
    }
    for (byte i = 0; i < 7; i++) {
      if (bitRead(numbers[number[n]], 7 - i) == HIGH) {
        digitalWrite(pinsCat[i], LOW);
      } else {
        digitalWrite(pinsCat[i], HIGH);
      }
    }
    analogWrite(pinsAn[n], 255);
    delay(bright);
    analogWrite(pinsAn[n], 0);
  }
}

//===========================================================
// Подсчет оборотов
void rpmMin() {
  timeCountTemp--;
  timeTemp += timeI;
  if (!timeCountTemp) {
    timeCountTemp = timeCount;
    randFig = 60 * (1000000 / (timeTemp / timeCount));
    timeTemp = 0;
  }
}

//===========================================================
// Программа вывода температуры
void temp() {
  figure();
  ds.reset();
  ds.write(0xCC);
  ds.write(0x44);

  figure();

  ds.reset();
  ds.write(0xCC);
  ds.write(0xBE);
  data[0] = ds.read();
  data[1] = ds.read();
  randFig = ((data[1] << 8) | data[0]) * 0.0625 * 10;
}

//===========================================================
void interrupt() {
  timeI = (micros() - time_last);
  time_last = micros();
}

//===========================================================
void setup() {
  // Сконфигурировать контакты как выходы
  for (byte i = 0; i < 7; i++) {
    pinMode(pinsCat[i], OUTPUT);
  }
  pinMode(button, INPUT_PULLUP);
  attachInterrupt(0, interrupt, FALLING);

  timeCountTemp = timeCount;
}

//===========================================================
void loop() {
  //  Обработка нажатия кнопки
  if (!digitalRead(button)) {
    flag = true;
  }
  if (digitalRead(button) && flag == true) {
    buttonState = !buttonState;
    flag = false;
  }

  //============================
  // В зависимости от кнопки 1 или 2
  if (!buttonState) {
    rpmMin();
  } else {
    temp();
  }
  figure();
}

//===========================================================


//  //  Программа простого 4х разрядного счетчика
//  void randcount() {
//    if (millis() >= oldMillis + timer) {
//      number[3] += 1;
//      if (number[3] == 10) {
//        number[3] = 0;
//        number[2] += 1;
//      }
//      if (number[2] == 10) {
//        number[2] = 0;
//        number[1] += 1;
//      }
//      if (number[1] == 10) {
//        number[1] = 0;
//        number[0] += 1;
//      }
//      if (number[0] == 10) {
//        number[0] = 0;
//      }
//
//      oldMillis += timer;
//    }
//  }

//    // Программа вывода случайного 4х разрядного числа
//    if (millis() >= oldMillis + timer) {
//      randFig = random(0, 9999);
//      oldMillis += timer;
//    }