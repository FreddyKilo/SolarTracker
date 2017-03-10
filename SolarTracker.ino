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

int xPosition;
int yPosition;

int sensorValue = 0;
int adjInterval = 15; // The time for next angle adjustment in minutes

// WiFi credentials here

//const char* host = "dweet.io";
WiFiClient client;

String responseJson;
int hour;

/*
 * Run once upon startup, or after returning from sleep
 */
void setup() {
  Serial.begin(115200);

  // Get the time of day from a simple GET request to dweet.io
  // We need to check to see if the sun is close to setting
  int sleepTime = 5; // Set to 60 seconds for testing

  // Connect to wifi and server
  WiFi.begin(ssid, password);
  if (!client.connect("dweet.io", 80)) {
    Serial.println("connection failed");
    return;
  }
  
//  sendDweet("x-position=" + xPosition + "&y-position=" + yPosition);

  delay(2000);

  getDweet();

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
//  Serial.println("Going to sleep...");
//  ESP.deepSleep(sleepTime * 1000000);
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
  ESP.deepSleep(1000); // Sleep for a millis. We just want to reset the device because we never got the time, which we really need.

  // See if we can find a higher reading in X (east/west)
  // Record the direction of movement from origin (pos or neg) and assume the next
  // movement should be in the same direction
  adjustAngleX();

  // Once we're good in X, let's find the highest reading for Y (north/south)
  // Record direction of movement for Y
  adjustAngleY();

  Serial.println(sensorValue);
}

/*
 * Send some data to dweet.io
 * 
 * @param params - The URL parameters to include with the request. i.e. "position=72&value=1500"
 * This data will be stored and can later be retrieved as Json using getDweet()
 */
void sendDweet(String params) {
  Serial.println();
  Serial.println("Sending dweet");
  client.print(String("GET /dweet/for/mr-roboto?" + params) + " HTTP/1.1\r\n" +
               "Host: " + "dweet.io" + "\r\n" +
               "Connection: close\r\n\r\n");
  parseResponse();
}

void getDweet() {
  Serial.println();
  Serial.println("Getting dweet");
  client.print(String("GET /get/latest/dweet/for/mr-roboto") + " HTTP/1.1\r\n" +
               "Host: " + "dweet.io" + "\r\n" +
               "Connection: close\r\n\r\n");
  parseResponse();
}


/*
 * Get the hour and the json String from a GET request
 */
void parseResponse() {
  delay(100);
  while (client.available()) {
    String line = client.readStringUntil('\r');
    // Get the time
    if (line.indexOf("Date:") > -1) {
      Serial.println("Got the time of day and Hour!");
      String timeOfDay = getSubstring(line, ' ', 5);
      // If we didn't get the time, return and call loop() to reset device
      if (timeOfDay.length() == 0) {
        return;
      }
      hour = (getSubstring(timeOfDay, ':', 0).toInt() + 17) % 24;
    }
    // Get the response Json
    if (line.indexOf('{') > -1) {
      Serial.println("Got the response Json!");
      responseJson = line;
      Serial.print(line);
    }
  }
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
   @returns - The substring at given index
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

