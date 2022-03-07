#include "config.h"

#define led 2
#define RXD2 16
#define TXD2 17

AdafruitIO_Feed *ledSelectFeed = io.feed("led-select");
AdafruitIO_Feed *sensorFeed = io.feed("sensor");

int flag = 0;
int sensorCommaRead = 0;
int sensorRead = 0;
float sensorWrite = 0;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  
  while(! Serial);

  pinMode(led, OUTPUT);

  Serial.print("Connecting to Adafruit IO");

  io.connect();

  ledSelectFeed->onMessage(handleMessage);

  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  ledSelectFeed->get();

  Serial.println();
  Serial.println(io.statusText());
}

void loop() {
  io.run();
  sensorCommaRead = Serial2.read();

  if (sensorCommaRead == ',') {
    sensorRead = Serial2.read();
    sensorWrite = (sensorRead - 48) * 10;
    sensorRead = Serial2.read();
    sensorWrite = sensorWrite + (sensorRead - 48);
    sensorRead = Serial2.read();
    sensorWrite = sensorWrite + (sensorRead - 48) * 0.1;
  }
  
  Serial.print("sending -> ");
  Serial.println(sensorWrite);

  sensorFeed->save(sensorWrite);
  sensorWrite = 0;
  delay(3000);
}

void handleMessage(AdafruitIO_Data *data) {

  Serial.print("received <- ");
  Serial.println(data->value());

  if(*data->value() == '1') {
    Serial.println("LED ROJO ENCENDIDO");
    Serial2.write("1");
    digitalWrite(led, HIGH);
  } else if (*data->value() == '2'){
    Serial.println("LED AMARILLO ENCENDIDO");
    Serial2.write("2");
    digitalWrite(led, HIGH);
  } else if (*data->value() == '3'){
    Serial.println("LED VERDE ENCENDIDO");
    Serial2.write("3");
    digitalWrite(led, HIGH);
  } else {
    Serial.println("LEDS APAGADAS");
    Serial2.write("0");
    digitalWrite(led, LOW);
  }
}
