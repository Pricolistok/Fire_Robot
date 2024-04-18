#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#define CLK 2
#define DIO 3
#define CLK1 7
#define DIO1 6
#include "GyverTM1637.h"
GyverTM1637 disp(CLK, DIO);
GyverTM1637 disp1(CLK1, DIO1);

RF24 radio(9, 10); 

byte recieved_data[3]; 
byte relay = 4;        



byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

void setup() {
  Serial.begin(9600);       // открываем порт для связи с ПК

  pinMode(relay, OUTPUT);   // настроить пин реле как выход

   disp.clear();
  disp.brightness(7);
     disp1.clear();
  disp1.brightness(7);
  radio.begin(); //активировать модуль
  radio.setAutoAck(1);        // режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    // (время между попыткой достучаться, число попыток)
  radio.enableAckPayload();   // разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);   // размер пакета, в байтах

  radio.openReadingPipe(1, address[0]);    
  radio.setChannel(0x65);  // выбираем канал 

  radio.setPALevel (RF24_PA_MAX);   // уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); // скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp();          // начать работу
  radio.startListening();   // начинаем слушать эфир, мы приёмный модуль
}

void loop() {
  byte pipeNo;
  
  while ( radio.available(&pipeNo)) { // есть входящие данные
    // чиатем входящий сигнал
    radio.read(&recieved_data, sizeof(recieved_data));
      Serial.print("   FLAME       ");
    // подать на реле сигнал с 0 места массива
    digitalWrite(relay, recieved_data[0]);
    Serial.print(recieved_data[0]);

      Serial.print("   GAZ       ");
    Serial.print(recieved_data[1]);
    disp.displayInt(recieved_data[1]);

  Serial.print("   TEMP       ");
    Serial.print(recieved_data[2]);
    disp1.displayInt(recieved_data[2]);
    delay(500);
  }
}
