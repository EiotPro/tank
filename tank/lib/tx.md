#include <Arduino.h>

#define RAK3172_RESET 2

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);

  pinMode(RAK3172_RESET, OUTPUT);
  digitalWrite(RAK3172_RESET, HIGH);
  delay(2000);

  Serial.println("TX Node Started");

  // LoRa P2P config
  Serial1.println("AT+NWM=0");
  delay(200);
  Serial1.println("AT+P2P=865000000:7:0:1:8:14");
  delay(200);

  // TX doesn’t need RX
  Serial1.println("AT+PRECV=0");
  delay(200);
}

void loop() {
  Serial1.println("AT+PSEND=05");
  
  Serial.println("Sent: 05");
  delay(3000);  // Send every 3 seconds
}
