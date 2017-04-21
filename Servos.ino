
/*
	Channel 5
	Min: 1168 (20 deg)
	Max: 1744 (160 deg)
*/
void setServoX(int degrees) {
	int target = getTarget(20, 160, 1168, 1744, degrees);
	int quarterMicroSec = target * 4;

	Serial.println("Moving servo x to " + String(degrees));
	maestro.setTarget(5, quarterMicroSec);

	delay(xIncrement * 35);
	xPosition = degrees;
}

/*
	Channel 4
	Min: 1408 (90 deg)
	Max: 2000 (150 deg)
*/
void setServoY(int degrees) {
	int target = getTarget(90, 150, 1408, 2000, degrees);
	int quarterMicroSec = target * 4;

	Serial.println("Moving servo y to " + String(degrees));
	maestro.setTarget(4, quarterMicroSec);

	delay(yIncrement * 40);
	yPosition = degrees;
}

/*
	Get the servo target using degrees as input
*/
int getTarget(int x1, int x2, int y1, int y2, int input) {
	int slope = (y2 - y1) / (x2 - x1);
	int yIntercept = y1 - slope * x1;
	return slope * input + yIntercept;
}

bool servoIsMoving() {
	// TODO: Make this work asap	
	return false;
}

void setNeutralPosition() {
	setServoX(90);
	setServoY(90);
	// Wait for servos to stop or else we could get a battery reading with a significant voltage drop
	delay(4000);
}

/*
	Get the x position relative to the current time
	@returns - angle in degrees
*/
int getRelativePositionX() {
	return (hour - 6) * 15;
}

/*
	Set the position of the servos to where they should approximately be according to the time of day
	@param offset - the amount of offset in degrees
*/
void setRelativePositionX(int offset) {
	int degrees = getRelativePositionX() + offset;
	if (degrees <= xMax) {
		setServoX(degrees);
	} else {
		setServoX(xMax);
	}
	delay(3000);
}

void setOptimalPostion() {
	int firstReading;
	int secondReading;

	// Adjust x axis until highest value is found
	while (maxLightValue <= 825 && xPosition < xMax) {
		firstReading = getLightValue();
		setServoX(xPosition + xIncrement);
		secondReading = getLightValue();

		// If readings start to decrease, move back to last greatest reading
		if (secondReading < firstReading && firstReading >= minLightValue) {
			setServoX(xPosition - xIncrement);
			break;

		// If the current x position ends up greater than the relative positive threshold
		// aka if we pass where the sun should have been, go back to where we think it is
		} else if (xPosition > getRelativePositionX() + relativeThreshold) {
			setRelativePositionX(0);
			break;
		}
	}

	// Start at 2 increments behind previous position to account for southern directional variation
	if (yPosition - yIncrement * 2 >= yMin) {
		yPosition -= yIncrement * 2;
	} else {
		yPosition = yMin;
	}
	setServoY(yPosition);
	
	// Adjust y axis until highest value is found
	while (maxLightValue < 830 && yPosition < yMax) {
		firstReading = getLightValue();
		setServoY(yPosition + yIncrement);
		secondReading = getLightValue();

		if (secondReading < firstReading && firstReading >= minLightValue) {
			setServoX(yPosition - yIncrement);
			break;
		}
	}
}
