#include <Arduino_MKRIoTCarrier.h>
MKRIoTCarrier carrier;

#define IRSensorpin A1

void setup() {
  carrier.noCase();
  carrier.begin();
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(IRSensorpin,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int var = digitalRead(IRSensorpin);
  Serial.println(var);
  if (var == 0){
    carrier.Buzzer.beep();
  }
}
