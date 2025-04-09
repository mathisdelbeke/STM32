#include <Wire.h>

String incomingData = "";      // To store incoming data

void setup() {
  Wire.begin(0x08); // Set I2C address
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.begin(9600);
}

void loop() {
  delay(100);
}

void receiveEvent(int howMany) {
  incomingData = "";
  while (Wire.available()) {
    char c = Wire.read();
    incomingData += c;
  }
  Serial.println(incomingData);
}

void requestEvent() {
  Wire.write("Hey STM32\r\n");
}
