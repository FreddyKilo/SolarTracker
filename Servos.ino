
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

// TODO: This needs to work asap
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

/*
	Set the position of the servos to where they should approximately be according to the time of day
*/
void setRelativePositionX(int offset) {
	setServoX(relativeAngleX + offset);
	delay(2000);
}

void setOptimalPostion() {
	int firstReading;
	int secondReading;
	while (maxLightValue <= 825 && xPosition < 160) {
		firstReading = getLightValue();
		setServoX(xPosition + xIncrement);
		secondReading = getLightValue();

		// x scan has finished, move to optimal position
		if (secondReading < firstReading && firstReading >= minLightValue) {
			setServoX(xPosition - xIncrement);
			break;
		} else if (xPosition > relativeAngleX + 20) {
			setServoX(relativeAngleX);
			break;
		}

		// // Check if the peak could have been close to halfway between previous and current
		// // If so, move back one half of the interval
		// if ((firstReading < maxLightValue + 4 && firstReading > maxLightValue - 4)
		// && firstReading >= minLightValue) {
		// 	setServoX(xPosition - xIncrement / 2);
		// 	break;
		// // Peak was closer to the previous reading, move back to it
		// } else if (firstReading >= secondReading) {
		// 	setServoX(xPosition - xIncrement);
		// 	break;
		// }
	}

	// Adjust y axis until highest value is found
	// Start at 2 increments behind previous position to account for southern directional variation
	if (yPosition - yIncrement >= 90) {
		yPosition -= yIncrement;
	} else {
		yPosition = 90;
	}
	setServoY(yPosition);
	
	while (maxLightValue < 830 && yPosition < 130) {
		firstReading = getLightValue();
		setServoY(yPosition + yIncrement);
		secondReading = getLightValue();

		if (secondReading < firstReading && firstReading >= minLightValue) {
			setServoX(yPosition - yIncrement);
			break;
		}

		// if ((currentReading < maxLightValue + 2 && currentReading > maxLightValue - 2)
		// && currentReading >= minLightValue) {
		// 	setServoY(yPosition - yIncrement / 2);
		// 	break;
		// } else if (currentReading < maxLightValue) {
		// 	setServoY(yPosition - yIncrement);
		// 	break;
		// }
	}
}
