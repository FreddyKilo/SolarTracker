
/*
	Channel 4
	Min: 1408 (90 deg)
	Max: 2000 (150 deg)
*/
void setServoY(int degrees) {
	int channel = 4;
	int target = ((degrees - 90) * 10) + 1408;
	int quarterMicroSec = target * 4;
	Serial.println("Moving servo y to " + String(degrees));
	maestro.setTarget(channel, quarterMicroSec);
	delay(yIncrement * 40);
	yPosition = degrees;
}

/*
	Channel 5
	Min: 1168 (20 deg)
	Max: 1744 (160 deg)
*/
void setServoX(int degrees) {
	int channel = 5;
	int target = ((degrees - 20) * 4) + 1168;
	int quarterMicroSec = target * 4;
	Serial.println("Moving servo x to " + String(degrees));
	maestro.setTarget(channel, quarterMicroSec);
	delay(xIncrement * 35);
	xPosition = degrees;
}

bool servoIsMoving() {
	if (maestro.getMovingState() == 1) {
		Serial.println("Servo is moving");
		delay(100);
		return true;
	}
	return false;
}

void setHomePosition() {
 	int delayTime;
 	// If it's past noon, we want to start at roughly 90 deg in the x axis
	if (hour >= 13) {
		setServoX(90);
		delayTime = 2000;
	} else {
		setServoX(30);
		delayTime = 4000;
	}
	setServoY(90);
	delay(delayTime);
}

void setNeutralPosition() {
	setServoX(90);
	setServoY(90);
}
