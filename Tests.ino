
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
  getData();
  parseResponse();
  calculateSolarPosition();
  delay(60000); 
}

void testPubNub() {
  connectToPubNub();
  client.print(String("GET /publish/pub-c-3b242e5e-131b-4684-aba1-aa869f2f0be5/sub-c-98b69416-3f65-11e7-a9cb-02ee2ddab7fe/test/hello_earth/test/%22Hello%20World%22") +
                      " HTTP/1.1\r\n" +
                      "Host: " + "pubsub.pubnub.com" + "\r\n" +
                      "Connection: close\r\n\r\n");
  while (client.available()) {
    String line = client.readStringUntil('\r');
    // Print each line from the server response
    Serial.println(line);
  }

}
