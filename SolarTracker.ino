
#include <PololuMaestro.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <math.h>

WiFiClient client;

int xPosition;
int yPosition;

// These are set for mechanical limitations to the X and Y axes
int xMin = 20;
int xMax = 160;
int yMin = 90;
int yMax = 135;

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

unsigned long oneSecond = 1030000L; // The ESP8266 RTC is a little off and we need to compensate for it
int daySleepTime = 900 * oneSecond;   // 600 sec = 10 min
int nightSleepTime = 3600 * oneSecond; // 3600 sec = 1 hour

bool stationaryMode = false;
bool connected;
bool debug = true;

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
	WiFi.begin(getSsid(credCode), getPassword(credCode));
}

void loop() {
	// Make a request to dweet.io for stored data
	getData();
	// Pick out all the valuable data we need from the server response
	parseResponse();
	// Set up the time variables to be able to format to a human readable time
	setupReadableTime();

	startTracking();

	// runTests();
}

/*
	Test method
*/
void runTests() {
	delay(2000);
}
