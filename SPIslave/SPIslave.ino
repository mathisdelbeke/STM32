#include <SPI.h>

volatile boolean received = false;
char data[10];
byte index = 0;

void setup() {
  pinMode(MISO, OUTPUT); // Have to send data to master
  SPCR |= _BV(SPE);      // Enable SPI
  SPI.attachInterrupt(); // Enable SPI interrupt
  Serial.begin(9600);
}

ISR(SPI_STC_vect) {
  data[index++] = SPDR;  // Read byte from master
  received = true;
}

void loop() {
  if (received) {
    received = false;
    data[index] = '\0';  // Null-terminate
    Serial.print("Received: ");
    Serial.println(data);
    index = 0;
  }
}
