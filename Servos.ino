
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
 	// If it's past noon, we want to start at roughly 90 deg in the x axis
	if (hour > 12) {
		setServoX(90);
	} else {
		setServoX(30);
	}
	setServoY(90);
	delay(2000);
}

void setOptimalPostion() {
	
	int currentReading;
	while (maxLightValue <= 825 && xPosition < 160) {
		setServoX(xPosition + xIncrement);
		// Check if the peak could have been close to halfway between previous and current
		// If so, move back one half of the interval
		currentReading = getLightValue();
		if ((currentReading < maxLightValue + 4 && currentReading > maxLightValue - 4)
		&& currentReading >= minLightValue) {
			setServoX(xPosition - xIncrement / 2);
			break;
		// Peak was closer to the previous reading, move back to it
		} else if (currentReading < maxLightValue && currentReading >= minLightValue) {
			setServoX(xPosition - xIncrement);
			break;
		}
	}

	// Adjust y axis until highest value is found
	// Start at 2 increments behind previous position to account for southern directional variation
	if (yPosition - (yIncrement * 2) >= 90) {
		yPosition -= yIncrement * 2;
	} else {
		yPosition = 90;
	}
	setServoY(yPosition);
	
	while (maxLightValue < 830 && yPosition < 130) {
		setServoY(yPosition + yIncrement);

		currentReading = getLightValue();
		if ((currentReading < maxLightValue + 2 && currentReading > maxLightValue - 2)
		&& currentReading >= minLightValue) {
			setServoY(yPosition - yIncrement / 2);
			break;
		} else if (currentReading < maxLightValue) {
			setServoY(yPosition - yIncrement);
			break;
		}
		maxLightValue = getLightValue();
	}
}
