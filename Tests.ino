
void testReadings() {
  Serial.println("Voltage: " + String(getVoltage()));
  delay(400);

  Serial.println("Light: " + String(getLightValue()));
  delay(400);

  Serial.println("Current: " + String(getCurrent()));
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
  setAzimuth(120, 0);
  setElevation(50, 0);
  setAzimuth(160, 0);
  setElevation(70, 0);
  setAzimuth(200, 0);
}

void testCalculations() {
  getData();
  parseResponse();
  calculateSolarPosition();
  delay(60000); 
}
