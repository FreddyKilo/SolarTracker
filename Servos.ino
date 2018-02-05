
/*
	Channel 5
	90 deg = 1894
	270 deg = 1124
*/
void setAzimuth(float degrees, int waitMillis) {
	int target = map(degrees, 90, 270, 1894, 1124);
	int quarterMicroSec = target * 4;

	Serial.println("Moving servo x to " + String(degrees));
	maestro.setTarget(5, quarterMicroSec);

	delay(waitMillis);
	xPosition = degrees;
}

/*
	Channel 3
	90 deg = 1472
	0 deg = 2366
*/
void setElevation(float degrees, int waitMillis) {
	int target = map(degrees, 0, 90, 2366, 1408);
	int quarterMicroSec = target * 4;

	Serial.println("Moving servo y to " + String(degrees));
	maestro.setTarget(3, quarterMicroSec);

	delay(waitMillis);
	yPosition = degrees;
}

bool servoIsMoving() {
	// TODO: Make this work asap	
	return false;
}

void setNeutralPosition() {
	setAzimuth(180, 0);
	setElevation(90, 4000);
}

void setStationaryPosition() {
	setAzimuth(180, 0);
	setElevation(76, 0);
}
