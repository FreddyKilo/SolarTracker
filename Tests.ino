
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
  setAzimuth(120, 0);
  setElevation(50, 0);
  setAzimuth(160, 0);
  setElevation(70, 0);
  setAzimuth(200, 0);
}

void testCalculations() {
  int dayOfCycle = getDaysSinceSpringEquinox(130);
  float tilt = sin(dayOfCycle * .01676) * 23.5;
  float lod = getLengthOfDay(33.4484, tilt);
  Serial.println("length of day: " + String(lod));
  delay(10000);
}
