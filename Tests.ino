
void testReadings() {
  Serial.println("Voltage: " + String(getVoltage()));
  delay(400);

  Serial.println("Light: " + String(getLightValue()));
  delay(400);
}

void testJson() {
  getData();
  parseResponse();
  Serial.println("Value from \"voltage\": " + getJsonValue("voltage"));
  Serial.println("Value from \"sunset\": " + getJsonValue("sunset"));
  Serial.println("Value from \"time\": " + getJsonValue("time"));
  Serial.println("Value from \"light\": " + getJsonValue("light"));
  delay(5000);
}

void testServos() {
  setServoY(100);
  delay(1000);
  setServoX(80);
  delay(1000);
  setServoY(120);
  delay(1000);
  setServoX(100);
  delay(1000);
}
