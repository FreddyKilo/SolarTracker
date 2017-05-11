
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
  setNeutralPosition();
  setAzimuth(120);
  setElevation(50);
  setAzimuth(160);
  setElevation(70);
  setAzimuth(200);
}

void testCalculations() {
  int t = map(90, 90, 270, 1894, 1124);
  Serial.println("target: " + String(t));
  delay(10000);
}
