/*
	##### ToDo list #####

	Prototype servo
*/

#include <Servo.h>
#include <ESP8266WiFi.h>

WiFiClient client;
Servo servoX;
Servo servoY;

// Values to store for later use
int xPosition;
int yPosition;
String dir;

String jsonContent;
String timeOfDay = "";
int hour;
int twelveHour;
int minutes;
String timeSuffix = "";
int sleepTime = 300; // 300 sec = 5 min
unsigned long oneSecond = 1000000L;

/*
	Run this for test
*/
void setupx() {
	Serial.begin(115200);
}

void setup() {
	Serial.begin(115200);
	Serial.println();

	pinMode(D3, OUTPUT);
	pinMode(D4, OUTPUT);

	// Connect to WiFi network
	//    WiFi.begin(ssid, pass);

	/*
		 Get the values from the last dweet then use those values to help adjust the position
		 of the solar panels to obtain max voltage
	*/

	getData();
	parseResponse();

	if (timeOfDay.equals("")) {
		ESP.reset();
	}

	// Let's use the 12 hour clock system :)
	if (hour > 11) {
		twelveHour = hour;
		timeSuffix = "p";
		if (hour > 12) {
			twelveHour = hour % 12;
		}
	}

	// If minutes are less than 10, we need to add a zero before the number i.e. 8 = 08
	String strMinutes;
	if (minutes < 10) {
		strMinutes = "0" + String(minutes);
	} else {
		strMinutes = String(minutes);
	}

	// If waking up from sunset, reset to morning position.
	// If no morning postion, do an initial scan
	// Else just get the previous readings
	if (getValue("sunset").equals("true") && getValue("morningX").length() > 0) {
		xPosition = getValue("morningX").toInt();
		yPosition = getValue("morningY").toInt();
	} else if (getValue("sunset").equals("true")) {
		initialScan();
	} else {
		xPosition = getValue("x").toInt();
		yPosition = getValue("y").toInt();
		//    dir = getValue("direction");
		dir = "pos";
	}

	// If the hour is greater than 6pm (about sunset) and less than 7am (about sunrise), stop
	// tracking and get the voltage reading periodically to monitor on freeboard
	if (hour < 7 || hour > 18) {
		setData("time=" + String(twelveHour) + ":" + strMinutes + timeSuffix +
				"&sunset=" +
				"&light=" +
				"&voltage=" + String(getVoltage()));
		Serial.println("See you in the morning...");
		ESP.deepSleep(1800 * oneSecond);
	}

	/*
		 Using previous values, make adjustments to get the most amount of sunlight possible
	*/

	// Get current light reading

	// If reading is less than last reading, we need to re-position. Use values from last reading
	// to adjust position. If the direction of the last reading was positive, check for a greater
	// reading in the positive direction. Keep track of light sensor value with coresponding servo
	// position. If there are more that 3 consecutive lesser readings, stop re-position and move to
	// servo position mapped to highest sensor value.

	//  servoX.attach(0);
	//  servoY.attach(1);
	//  servoX.write(xPosition);
	//  servoY.write(yPosition);

	/*
			Store current values for next position adjustment.
	*/

	setData("x=" + String(xPosition) +
			"&y=" + String(yPosition) +
			"&direction=" + dir +
			"&light=" + String(getLightValue()) +
			"&voltage=" + String(getVoltage()) +
			"&time=" + String(twelveHour) + ":" + strMinutes + timeSuffix +
			"&sunset=false");

	// Sleep until next scheduled adjustment
	Serial.println("");
	Serial.println("Taking a nap...");
	ESP.deepSleep(sleepTime * oneSecond);
}

/*
	 This will be called if there were no previous recorded servo positions.
*/
void initialScan() {
	// Move x-axis in the default direction while taking readings
	// Compare a few readings after movement
	// if the readings are decreasing, move in the other direction
	// if the readings are increasing, move until the reading reaches it's highest value
	// keep track of the servo position with the highest reading
	// once the reading starts to decrease after n readings move back to the highest position
}

/*
	 Send some data to dweet.io

	 @param params - The URL parameters to include with the request. i.e. "position=72&value=1500"
	 This data will be stored and can later be retrieved as Json using getData()
*/
void setData(String params) {
	connectToDweetServer();
	Serial.println("");
	Serial.println("Sending dweet");
	client.print(String("GET /dweet/for/mr-roboto?" + params) + " HTTP/1.1\r\n" +
							 "Host: " + "dweet.io" + "\r\n" +
							 "Connection: close\r\n\r\n");
}

/*
	 Make a call to dweet.io. Response is saved in <WiFiClient client> and is parsed with parseResponse()
*/
void getData() {
	connectToDweetServer();
	Serial.println("");
	Serial.println("Getting dweet");
	client.print(String("GET /get/latest/dweet/for/mr-roboto") + " HTTP/1.1\r\n" +
							 "Host: " + "dweet.io" + "\r\n" +
							 "Connection: close\r\n\r\n");
}

/*
	 Connect to dweet.io, this is where we will store our data
*/
void connectToDweetServer() {
	if (!client.connect("dweet.io", 80)) {
		Serial.println("connection failed");
		return;
	}
}

/*
	 Get the hour, minutes, and json String from a GET request
*/
void parseResponse() {
	unsigned long timeout = millis();

	while (client.available() == 0) {

		if (millis() - timeout > 5000) {
			Serial.println(">>> Client Timeout !");
			client.stop();
			sleepTime = 1;
			return;
		}
	}

	while (client.available()) {
		String line = client.readStringUntil('\r');

		// Get the time
		if (line.indexOf("Date:") > -1) {
			timeOfDay = getSubstring(line, ' ', 5);
			hour = (getSubstring(timeOfDay, ':', 0).toInt() + 17) % 24;
			minutes = (getSubstring(timeOfDay, ':', 1).toInt());
		}

		// Get the content from response Json
		if (line.indexOf('{') > -1) {
			jsonContent = line.substring(line.indexOf("\"jsonContent\":{") + 11, line.indexOf("}"));
		}
	}
}

/*
	 Get a value from the content of the response Json
	 @param key - The key associated with the value
	 @returns - The value
*/
String getValue(String key) {
	String value;

	for (int i = 0; i < 6; i++) {
		String subString = getSubstring(jsonContent, ',', i);

		if (subString.indexOf(key) == 1) {
			value = subString.substring(subString.indexOf(':') + 1);
		}
	}
	return value;
}

int getLightValue() {
	// switch to ldr sensor
	digitalWrite(D3, LOW);
	digitalWrite(D4, HIGH);
	delay(100);
	int sensorValue = analogRead(A0);
	int light = 1000 * sensorValue / 830;

	return light / 10;
}

float getVoltage() {
	// switch to battery input
	digitalWrite(D4, LOW);
	digitalWrite(D3, HIGH);
	delay(100);
	int sensorValue = analogRead(A0);

	float voltage = sensorValue * (4.2 / 713); // With our voltage divider, we get a reading of 710 at 4.2v;
	return voltage;
}

void adjustAngleX() {

}

void adjustAngleY() {

}

/*
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

void loop() {
	test();

	// See if we can find a higher reading in X (east/west)
	// Record the direction of movement from origin (pos or neg) and assume the next
	// movement should be in the same direction
	// adjustAngleX();

	// Once we're good in X, let's find the highest reading for Y (north/south)
	// Record direction of movement for Y
	// adjustAngleY();
}

void test() {
	Serial.println("Voltage: " + String(getVoltage()));
	delay(400);

	Serial.println("Light: " + String(getLightValue()));
	delay(400);
}

