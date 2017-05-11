
#include <PololuMaestro.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <math.h>

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
int year;
int month;
int day;
int hour;
int gmtHour;
int twelveHour;
int minutes;

String jsonContent;
String strMinutes;
String timeSuffix = "a";
String timeOfDay = "";

int daySleepTime = 1200;   // 600 sec = 10 min
int nightSleepTime = 3600; // 3600 sec = 1 hour
unsigned long oneSecond = 1000000L;

bool debugMode = false;
bool connected;

// Servo controller communication
SoftwareSerial maestroSerial(D6, D5);
MicroMaestro maestro(maestroSerial);

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
}

void loop() {
	startTracking();
	// runTests();
}

/*
	Test method
*/
void runTests() {
	// Make a request to dweet.io for stored data
	getData();
	parseResponse();
	setupReadableTime();
	calculateSolarPosition();
	printCalculations();
	setData("x=" + String(xPosition) +
				"&y=" + String(yPosition) +
				"&light=" + String(getLightValue()) +
				"&voltage=" + String(getVoltage()) +
				"&time=" + getReadableTime() +
				"&sunset=false");

	delay(60000);
}
