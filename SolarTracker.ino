/*
	##### ToDo list #####
	Prototype servo
*/

#include <PololuMaestro.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

WiFiClient client;

// Values to store for later use
int xPosition;
int yPosition;
int xIncrement = 10;
int yIncrement = 10;
String dir;

String jsonContent;
String timeOfDay = "";
int hour;
int twelveHour;
int minutes;
String timeSuffix = "a";
int sleepTime = 1200; // 300 sec = 5 min
int overnightSleepTime = 1800; // 3600 sec = 1 hour
unsigned long oneSecond = 1000000L;

// Servo controller communication
SoftwareSerial maestroSerial(12, 14);
MicroMaestro maestro(maestroSerial);

/*
	Setup for test
*/
void setupx() {
	Serial.begin(115200);
	maestroSerial.begin(9600);
}

/*
	Setup for application
*/
void setup() {
	Serial.begin(115200);
	maestroSerial.begin(9600);

	/* 
		Declare all pins used for swiching inputs to analogRead(A0)
		D2 - switch for current from solar panel reading
		D3 - switch for battery voltage reading
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
	go();
}

void loop() {
	testServos();

	// See if we can find a higher reading in X (east/west)
	// Record the direction of movement from origin (pos or neg) and assume the next
	// movement should be in the same direction
	// setServoX();

	// Once we're good in X, let's find the highest reading for Y (north/south)
	// Record direction of movement for Y
	// setServoY();
}

void testReadings() {
	// Serial.println("Voltage: " + String(getVoltage()));
	// delay(400);

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

