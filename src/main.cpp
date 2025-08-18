#include <Arduino.h>
#define RAK3172_RESET 2

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);

  pinMode(RAK3172_RESET, OUTPUT);
  digitalWrite(RAK3172_RESET, HIGH);

  delay(2000);

#ifdef ROLE_TX
  Serial.println("TX Node Started");
  Serial1.println("AT+NWM=0");
  delay(200);
  Serial1.println("AT+P2P=865000000:7:0:1:8:14");
  delay(200);
  Serial1.println("AT+PRECV=0");   // TX doesn’t need RX
  delay(200);
#endif

#ifdef ROLE_RX
  Serial.println("RX Node Started");
  Serial1.println("AT+NWM=0");
  delay(200);
  Serial1.println("AT+P2P=865000000:7:0:1:8:14");
  delay(200);
  Serial1.println("AT+PRECV=65535");   // Enable RX
  delay(200);
#endif
}

void loop() {
#ifdef ROLE_TX
  Serial1.println("AT+PSEND=05");
  Serial.println("Sent: 05");
  delay(3000);
#endif

#ifdef ROLE_RX
  while (Serial1.available()) {
    String resp = Serial1.readStringUntil('\n');
    resp.trim();
    if (resp.length() > 0) {
      Serial.println("RX => " + resp);
      if (resp.startsWith("+EVT:RXP2P")) {
        delay(100);
        Serial1.println("AT+PRECV=65535");
      }
    }
  }
#endif
}
