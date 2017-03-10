/**
   ##### ToDo list #####

    Sleep from sundown to sunrise
    Log positions with datetime
    Test current draw from system
    Prototype servo

*/

#include <Servo.h>
#include <ESP8266WiFi.h>

Servo servoX;
Servo servoY;

// Values we want to store on dweet.io
int xPosition;
int yPosition;
String dir;

int sensorValue = 0;
int adjInterval = 15; // The time for next angle adjustment in minutes

// WiFi credentials here

WiFiClient client;

String content;
int hour;

/*
   Run once upon startup, or after returning from sleep
*/
void setup() {
  Serial.begin(115200);
  Serial.println();

  int sleepTime = 10; // Set to 60 seconds for testing

  // Connect to WiFi network
  //  WiFi.begin(ssid, password);

  getDweet();
  xPosition = parseContent("x").toInt();
  yPosition = parseContent("y").toInt();
  Serial.println("x: " + String(xPosition));
  Serial.println("y: " + String(yPosition));
  
  xPosition++;
  yPosition++;

  sendDweet("x=" + String(xPosition) + "&y=" + String(yPosition) + "&direction=" + dir);

  // If we got a response with the date and time from dweet, parse hours and minutes
  // If no response, set sleep time to one second and try again
  //  if (timeOfDay.length() > 0) {
  //    Serial.print("Hour: ");
  //    Serial.println(getHour(timeOfDay));
  //
  //    Serial.print("Minutes: ");
  //    Serial.println(getMinutes(timeOfDay));
  //
  //    Serial.print("Time: ");
  //    Serial.println(timeOfDay);
  //  } else {
  //    sleepTime = 1;
  //    Serial.println("timeOfDay was empty...");
  //  }

  // Set servos to neurtal position
  //  xPosition = 90;
  //  yPosition = 90;
  //  servoX.attach(0);
  //  servoY.attach(1);
  //  servoX.write(xPosition);
  //  servoY.write(yPosition);

  // Get initial reading of sensor
  //  sensorValue = analogRead(A0);
  // Scan to find the highest light reading
  //  initialScan();
  Serial.println("");
  Serial.println("Good night...");
  ESP.deepSleep(sleepTime * 1000000);
}

void initialScan() {
  // Move x-axis in the default direction while taking readings
  // Compare a few readings after movement
  // if the readings are decreasing, move in the other direction
  // if the readings are increasing, move until the reading reaches it's highest value
  // keep track of the servo position with the highest reading
  // once the reading starts to decrease after n readings move back to the highest position
}

void loop() {

  // See if we can find a higher reading in X (east/west)
  // Record the direction of movement from origin (pos or neg) and assume the next
  // movement should be in the same direction
  adjustAngleX();

  // Once we're good in X, let's find the highest reading for Y (north/south)
  // Record direction of movement for Y
  adjustAngleY();

  //  Serial.println(sensorValue);
}

/*
   Send some data to dweet.io

   @param params - The URL parameters to include with the request. i.e. "position=72&value=1500"
   This data will be stored and can later be retrieved as Json using getDweet()
*/
void sendDweet(String params) {
  connectToDweetServer();
  Serial.println("");
  Serial.println("Sending dweet");
  client.print(String("GET /dweet/for/mr-roboto?" + params) + " HTTP/1.1\r\n" +
               "Host: " + "dweet.io" + "\r\n" +
               "Connection: close\r\n\r\n");
  parseResponse();
}

void getDweet() {
  connectToDweetServer();
  Serial.println("");
  Serial.println("Getting dweet");
  client.print(String("GET /get/latest/dweet/for/mr-roboto") + " HTTP/1.1\r\n" +
               "Host: " + "dweet.io" + "\r\n" +
               "Connection: close\r\n\r\n");
  parseResponse();
}

void connectToDweetServer() {
  if (!client.connect("dweet.io", 80)) {
    Serial.println("connection failed");
    return;
  }
}

/*
   Get the hour and the json String from a GET request
*/
void parseResponse() {
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  while (client.available()) {
    String line = client.readStringUntil('\r');
    // Get the time
    if (line.indexOf("Date:") > -1) {
      String timeOfDay = getSubstring(line, ' ', 5);
      Serial.println("timeOfDay: " + timeOfDay);
      hour = (getSubstring(timeOfDay, ':', 0).toInt() + 17) % 24;
      Serial.println("hour: " + String(hour));
    }
    // Get the content from response Json
    if (line.indexOf('{') > -1) {
      content = line.substring(line.indexOf("\"content\":{") + 11, line.indexOf("}"));
    }
  }
}

String parseContent(String key) {
  String value;
  for (int i = 0; i < 3; i++) {
    String subString = getSubstring(content, ',', i);
    if (subString.indexOf(key) == 1) {
      value = subString.substring(subString.indexOf(':') + 1);
    }
  }
  return value;
}

void adjustAngleX() {

}

void adjustAngleY() {

}

/**
   Got this from stack overflow https://goo.gl/HZIMaU
   Get a substring from a String.

   @param line - The original String
   @param separator - The char(s) at where to separate
   @param index - The index of the substring to return
   @returns - The substring at given index or empty String if not found
*/
String getSubstring(String line, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = line.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (line.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return (found > index ? line.substring(strIndex[0], strIndex[1]) : "");
}

