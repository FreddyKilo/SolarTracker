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
String timeSuffix = "a";
int sleepTime = 600; // 300 sec = 5 min
int overnightSleepTime = 1800; // 3600 sec = 1 hour
unsigned long oneSecond = 1000000L;

/*
	Setup for test
*/
void setupx() {
	Serial.begin(115200);
}

/*
	Setup for application
*/
void setup() {
	Serial.begin(115200);

	/* 
		Declare all pins used for swiching inputs to analogRead(A0)
		D2 - switch for current from solar panel reading
		D3 - switch for voltage reading
		D4 - switch for LDR reading
	*/
	pinMode(D2, OUTPUT);
	pinMode(D3, OUTPUT);
	pinMode(D4, OUTPUT);

	// Initially set all pins to off
	digitalWrite(D2, LOW);
	digitalWrite(D3, LOW);
	digitalWrite(D4, LOW);

	// Connect to WiFi network
	//    WiFi.begin(ssid, pass);

	/*
		Get the values from the last dweet then use those values to help adjust the position
		of the solar panels to obtain max voltage
	*/

	getData();
	parseResponse();

	if (timeOfDay.equals("")) {
		ESP.deepSleep(oneSecond);
	}

	// Let's use the 12 hour clock system :)
	if (hour > 11) {
		twelveHour = hour;
		timeSuffix = "p";
		if (hour > 12) {
			twelveHour = hour % 12;
		}
	} else if (hour == 0) {
		twelveHour = 12;
	} else {
		twelveHour = hour;
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
	if (getJsonValue("sunset").equals("true") && getJsonValue("morningX").length() > 0) {
		xPosition = getJsonValue("morningX").toInt();
		yPosition = getJsonValue("morningY").toInt();
	} else if (getJsonValue("sunset").equals("true")) {
		initialScan();
	} else {
		xPosition = getJsonValue("x").toInt();
		yPosition = getJsonValue("y").toInt();
		dir = getJsonValue("direction");
	}

	// If the hour is greater than 6pm (about sunset) and less than 7am (about sunrise), stop
	// tracking and get the voltage reading periodically to monitor on freeboard
	if (hour < 7 || hour > 18) {
		setData("time=" + String(twelveHour) + ":" + strMinutes + timeSuffix +
				"&sunset=true" +
				"&light=" +
				"&voltage=" + String(getVoltage()));
		Serial.println("See you in the morning...");
		ESP.deepSleep(overnightSleepTime * oneSecond);
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

	adjustAngleX(3);
	adjustAngleY(1);

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
	Make a call to dweet.io. Response is saved in <WiFiClient client> and is parsed 
	with parseResponse()
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

		// If the client times out, sleep 5 mins and try again
		if (millis() - timeout > 5000) {
			Serial.println(">>> Client Timeout !");
			ESP.deepSleep(300 * oneSecond);
		}
	}

	while (client.available()) {
		String line = client.readStringUntil('\r');
		// Serial.println(line);

		// Get the time
		if (line.indexOf("Date:") > -1) {
			timeOfDay = getSubstring(line, ' ', 5);
			hour = (getSubstring(timeOfDay, ':', 0).toInt() + 17) % 24;
			minutes = (getSubstring(timeOfDay, ':', 1).toInt());
		}

		// Get the content from response Json
		if (line.indexOf('{') > -1) {
			jsonContent = line.substring(line.indexOf("\"content\":{") + 11, line.indexOf("}"));
		}
	}
}

/*
	Get a value from the content of the response Json
	@param key - The key associated with the value
	@returns - The value
*/
String getJsonValue(String key) {
	String value;

	for (int i = 0; i < 6; i++) {
		String subString = getSubstring(jsonContent, ',', i);

		if (subString.indexOf(key) == 1) {
			value = subString.substring(subString.indexOf(':') + 1);
		}
	}
	return value;
}

float getCurrent() {
	// switch to battery current input
	digitalWrite(D2, HIGH);
	delay(100);
	int sensorValue = analogRead(A0);

	// Calculate current with reading, somehow
	float current = 1.0;

	digitalWrite(D2, LOW);
	return current;
}

float getVoltage() {
	// switch to battery voltage input
	digitalWrite(D3, HIGH);
	delay(100);
	int sensorValue = analogRead(A0);

	// At 4.2v (max battery voltage) we get a reading of around 710
	float voltage = 4.2 * sensorValue / 713;

	digitalWrite(D3, LOW);
	return voltage;
}

int getLightValue() {
	// switch to ldr sensor
	digitalWrite(D4, HIGH);
	delay(100);
	int sensorValue = analogRead(A0);

	// In direct sunlight we get a reading of 830 and want our dashboard to have a max value of 100
	int light = 100 * sensorValue / 830;

	digitalWrite(D4, LOW);
	return light;
}

void adjustAngleX(int degreeAmount) {
	xPosition = xPosition + degreeAmount;
	servoX.write(xPosition);
}

void adjustAngleY(int degreeAmount) {
	yPosition = yPosition + degreeAmount;
	servoY.write(yPosition);
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
	testJson();

	// See if we can find a higher reading in X (east/west)
	// Record the direction of movement from origin (pos or neg) and assume the next
	// movement should be in the same direction
	// adjustAngleX();

	// Once we're good in X, let's find the highest reading for Y (north/south)
	// Record direction of movement for Y
	// adjustAngleY();
}

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

