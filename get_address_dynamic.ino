#include <WiFi.h>
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  delay(500);
  Serial.print("RECEIVER MAC: ");
  Serial.println(WiFi.macAddress());
}
void loop() {}
  // put your main code here, to run repeatedly: