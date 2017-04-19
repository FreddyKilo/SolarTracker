
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

void setNeutralPosition() {
	setServoX(90);
	setServoY(90);
	// Wait for servos to stop or else we could get a battery reading with a significant voltage drop
	delay(2000);
}

void setHomePosition() {
 	int delayTime;
 	// If it's past noon, we want to start at roughly 90 deg in the x axis
	if (hour > 12) {
		setServoX(90);
		delayTime = 2000;
	} else {
		setServoX(30);
		delayTime = 4000;
	}
	setServoY(90);
	delay(delayTime);
}

void setOptimalPostion() {
	
	// Get current light value
	int previousReading = getLightValue();
	// Adjust x axis until hightest value is found
	while (previousReading <= 825 && xPosition < 150) {
		setServoX(xPosition + xIncrement);
		// Check if the peak could have been close to halfway between previous and current
		// If so, move back one half of the interval
		if ((getLightValue() < previousReading + 10 && getLightValue() > previousReading - 10)
		&& getLightValue() >= minLightValue) {
			setServoX(xPosition - xIncrement / 2);
			break;
		// Peak was closer to the previous reading, move back to it
		} else if (getLightValue() < previousReading && getLightValue() >= minLightValue) {
			setServoX(xPosition - xIncrement);
			break;
		}
		previousReading = getLightValue();
	}

	// Get current light value
	previousReading = getLightValue();
	// Adjust y axis until highest value is found
	// Start at an increment behind previous position
	if (yPosition - yIncrement >= 90) {
		yPosition -= yIncrement;
	} else {
		yPosition = 90;
	}
	setServoY(yPosition);
	while (previousReading < 830 && yPosition < 140) {
		setServoY(yPosition + yIncrement);
		if ((getLightValue() < previousReading + 2 && getLightValue() > previousReading - 2)
		&& getLightValue() >= minLightValue) {
			setServoY(yPosition - yIncrement / 2);
			break;
		} else if (getLightValue() < previousReading) {
			setServoY(yPosition - yIncrement);
			break;
		}
		previousReading = getLightValue();
	}
}
