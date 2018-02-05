
#include <PololuMaestro.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <math.h>

WiFiClient client;

int xPosition;
int yPosition;

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

unsigned long oneSecond = 1040000L; // The ESP8266 RTC is a little off and we need to compensate for it
int daySleepTime = 600 * oneSecond;   // 600 sec = 10 min
int nightSleepTime = 3600 * oneSecond; // 3600 sec = 1 hour

bool stationaryMode = false;
bool connected;
bool debug = false;

int credCode = 1;

// Servo controller communication
SoftwareSerial maestroSerial(-1, D7); // we only need to send data to the servo controller
MicroMaestro maestro(maestroSerial);

SoftwareSerial SIM800Serial(D5, D6);

/*
	Setup for application
*/
void setup() {
	Serial.begin(19200);
	SIM800Serial.begin(19200);
	maestroSerial.begin(9600);

	// Set up reset pin for SIM800 module
	pinMode(D1, OUTPUT); // reset

	// Declare all pins used for swiching inputs to analogRead(A0)
	// D2 - switch for current from solar panel reading
	// D3 - switch for battery voltage reading
	// D4 - switch for LDR reading
	pinMode(D2, OUTPUT);
	pinMode(D3, OUTPUT);
	pinMode(D4, OUTPUT);

	// Initially set all pins to off
	digitalWrite(D1, LOW);
	digitalWrite(D2, LOW);
	digitalWrite(D3, LOW);
	digitalWrite(D4, LOW);

	// First, try to connect to WiFi
	Serial.print("Obtaining WiFi connection");
	WiFi.mode(WIFI_STA);
	WiFi.begin(getSsid(credCode), getPassword(credCode));
	unsigned long timerStart = millis();
	while(millis() - timerStart < 10000) {
		blink(50, 200);
		Serial.print(".");
		if(WiFi.status() == WL_CONNECTED) {
			Serial.println("");
			Serial.print("WiFi IP address: ");
			Serial.println(WiFi.localIP());
			startWithWiFi();
		}
	}

	// If no WiFi connection, use cellular modem
	if(WiFi.status() != WL_CONNECTED) {
		startWithModem();
	}
	
}

void loop() {
	// runTests();
}

/*
	Test method
*/
void runTests() {
	restartModem();
	startUp();
	connect();
	sendGetRequestToDweet();

	delay(2000000);
}
