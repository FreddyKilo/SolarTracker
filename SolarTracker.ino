
/*
*/

#include <PololuMaestro.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

WiFiClient client;

int xPosition;
int yPosition;

int xMin = 20;
int xMax = 160;
int yMin = 90;
int yMax = 135;

int xIncrement = 8;
int yIncrement = 8;
int relativeThreshold = 20;

int maxLightValue = 0;
int minLightValue = 400;

String date;
int month;
int day;
int hour;
int twelveHour;
int minutes;

String jsonContent;
String strMinutes;
String timeSuffix = "a";
String timeOfDay = "";

int daySleepTime = 1200;   // 600 sec = 10 min
int nightSleepTime = 3600; // 3600 sec = 1 hour
unsigned long oneSecond = 1000000L;

bool debug = false;

// Servo controller communication
SoftwareSerial maestroSerial(D6, D5);
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

	// Declare all pins used for swiching inputs to analogRead(A0)
	// D2 - switch for current from solar panel reading
	// D3 - switch for battery voltage reading
	// D4 - switch for LDR reading
	pinMode(D2, OUTPUT);
	pinMode(D3, OUTPUT);
	pinMode(D4, OUTPUT);

	// Initially set all pins to off
	digitalWrite(D2, LOW);
	digitalWrite(D3, LOW);
	digitalWrite(D4, LOW);

	// Connect to WiFi network
	int credCode = 1;
	// WiFi.begin(getSsid(credCode), getPassword(credCode));
	startTracking();
}

/*
	This is used only for running tests
*/
void loop() {
	// Make a request to dweet.io for stored data
	getData();
	// Pick out all the valuable data we need from the server response
	parseResponse();
	// Set up the time variables to be able to format to a human readable time
	setupReadableTime();
	int xPos = getRelativePositionX();
	int yPos = getRelativePositionY();
	Serial.println("Month: " + String(month));
	Serial.println("Day: " + String(day));
	Serial.println("Relative X Position: " + String(xPos));
	Serial.println("Relative Y Position: " + String(yPos));
	delay(50000);
}
