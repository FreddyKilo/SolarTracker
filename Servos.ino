
/*
	Channel 5
	Min: 1168 (20 deg)
	Max: 1744 (160 deg)
*/
void setServoX(int degrees) {
	int target = getLinearOutput(20, 160, 1168, 1744, degrees);
	int quarterMicroSec = target * 4;

	Serial.println("Moving servo x to " + String(degrees));
	maestro.setTarget(5, quarterMicroSec);

	delay(abs(xPosition - degrees) * 35);
	xPosition = degrees;
}

/*
	Channel 4
	Min: 1408 (90 deg)
	Max: 2000 (150 deg)
*/
void setServoY(int degrees) {
	int target = getLinearOutput(90, 150, 1408, 2000, degrees);
	int quarterMicroSec = target * 4;

	Serial.println("Moving servo y to " + String(degrees));
	maestro.setTarget(4, quarterMicroSec);

	delay(abs(yPosition - degrees) * 40);
	yPosition = degrees;
}

/*
	Get the servo target using degrees as input
*/
int getLinearOutput(int x1, int x2, int y1, int y2, int input) {
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
	delay(2000);
}

/*
	Get the x position relative to the current time
	The Earth spins 1 degree every 4 minutes
	@returns - angle in degrees
*/
int getRelativePositionX() {
	return ((hour - 6) * 60 + minutes) / 4;
}

/*
	Get the y position relative to Phoenix, AZ
	Using quadratic vertex form
*/
int getRelativePositionY() {
	int latitudeInPhoenix = 33.4;
	int earthTilt = 23.5;
	return -.93 * pow(hour - 12, 2) + 90 + latitudeInPhoenix;
}

/*
	Set the position of the servo to where it should approximately be according to the time of day
	@param offset - the amount of offset in degrees
*/
void setRelativePositionX(int offset) {
	int degrees = getRelativePositionX() + offset;
	if (degrees > xMax) {
		setServoX(xMax);
	} else if (degrees < xMin) {
		setServoX(xMin);
	} else {
		setServoX(degrees);
	}
}

void setRelativePositionY(int offset) {
	int degrees = getRelativePositionY() + offset;
	if (degrees > yMax) {
		setServoY(yMax);
	} else if (degrees < xMin) {
		setServoY(xMin);
	} else {
		setServoY(degrees);
	}
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
		setServoY(yPosition - yIncrement * 2);
	} else {
		setServoY(yMin);
	}
	
	// Adjust y axis until highest value is found
	while (maxLightValue < 830 && yPosition < yMax) {
		firstReading = getLightValue();
		setServoY(yPosition + yIncrement);
		secondReading = getLightValue();

		if (secondReading < firstReading && firstReading >= minLightValue) {
			setServoY(yPosition - yIncrement);
			break;
		}
	}
}
