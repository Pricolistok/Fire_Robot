#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#define MQ2pin (0)
#include <microDS18B20.h>
MicroDS18B20<2> sensor;
RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно
//RF24 radio(9,53); // для Меги
int sensT;
int sensValue;
byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

int flamePin = 7;

byte transmit_data[3];  // массив, хранящий передаваемые данные
byte latest_data[3];    // массив, хранящий последние переданные данные
boolean flag;           // флажок отправки данных

void setup() {
  Serial.begin(9600);   
sensor.requestTemp();
  radio.begin();              // активировать модуль
  radio.setAutoAck(1);        // режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    // (время между попыткой достучаться, число попыток)
  radio.enableAckPayload();   // разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);   // размер пакета, в байтах

  radio.openWritingPipe(address[0]);  
  radio.setChannel(0x65);    

  radio.setPALevel (RF24_PA_MAX);   // уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); // скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS


  radio.powerUp();        //начать работу
  radio.stopListening();  //не слушаем радиоэфир, мы передатчик
}

void loop() {
  sensValue = analogRead(MQ2pin);
  sensT = sensor.getTemp();
  sensor.requestTemp();
  // инвертированный (!) сигнал с кнопки
  transmit_data[0] = !digitalRead(flamePin);
  Serial.print("   FLAME       ");
  Serial.print(transmit_data[0]);
  
  transmit_data[1] = sensValue;
  Serial.print("   GAZ       ");
  Serial.print(transmit_data[1]);

  transmit_data[2] = sensT;
  Serial.print("   TEMP       ");
  Serial.print(transmit_data[2]);

  // получить значение

  for (int i = 0; i < 3; i++) {               // в цикле от 0 до числа каналов
    if (transmit_data[i] != latest_data[i]) { // если есть изменения в transmit_data
      flag = 1;                               // поднять флаг отправки по радио
      latest_data[i] = transmit_data[i];      // запомнить последнее изменение
    }
  }

  if (flag == 1) {
    radio.powerUp();    // включить передатчик
    radio.write(&transmit_data, sizeof(transmit_data)); // отправить по радио
    flag = 0;           //опустить флаг
    radio.powerDown();  // выключить передатчик
  }
}
