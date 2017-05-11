
/*
	Channel 5
	90 deg = 1894
	270 deg = 1124
*/
void setAzimuth(float degrees) {
	int target = map(degrees, 90, 270, 1894, 1124);
	int quarterMicroSec = target * 4;

	Serial.println("Moving servo x to " + String(degrees));
	maestro.setTarget(5, quarterMicroSec);

	delay(abs(xPosition - degrees) * 35);
	xPosition = degrees;
}

/*
	Channel 2
	90 deg = 1472
	0 deg = 2366
*/
void setElevation(float degrees) {
	int target = map(degrees, 0, 90, 2366, 1408);
	int quarterMicroSec = target * 4;

	Serial.println("Moving servo y to " + String(degrees));
	maestro.setTarget(3, quarterMicroSec);

	delay(abs(yPosition - degrees) * 40);
	yPosition = degrees;
}

bool servoIsMoving() {
	// TODO: Make this work asap	
	return false;
}

void setNeutralPosition() {
	setAzimuth(180);
	setElevation(90);
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
	int latitude = 33.4;
	int earthTilt = 23.5;
	return sin(getRadians(hour * 15 - 90)) * latitude + 90;
}

/*
	Set the position of the servo to where it should approximately be according to the time of day
	@param offset - the amount of offset in degrees
*/
void setRelativePositionX(int offset) {
	int degrees = getRelativePositionX() + offset;
	if (degrees > xMax) {
		setAzimuth(xMax);
	} else if (degrees < xMin) {
		setAzimuth(xMin);
	} else {
		setAzimuth(degrees);
	}
}

void setRelativePositionY(int offset) {
	int degrees = getRelativePositionY() + offset;
	if (degrees > yMax) {
		setElevation(yMax);
	} else if (degrees < xMin) {
		setElevation(xMin);
	} else {
		setElevation(degrees);
	}
}

void setOptimalPostion() {
	int firstReading;
	int secondReading;

	// Adjust x axis until highest value is found
	while (maxLightValue <= 825 && xPosition < xMax) {
		firstReading = getLightValue();
		setAzimuth(xPosition + xIncrement);
		secondReading = getLightValue();

		// If readings start to decrease, move back to last greatest reading
		if (secondReading < firstReading && firstReading >= minLightValue) {
			setAzimuth(xPosition - xIncrement);
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
		setElevation(yPosition - yIncrement * 2);
	} else {
		setElevation(yMin);
	}
	
	// Adjust y axis until highest value is found
	while (maxLightValue < 830 && yPosition < yMax) {
		firstReading = getLightValue();
		setElevation(yPosition + yIncrement);
		secondReading = getLightValue();

		if (secondReading < firstReading && firstReading >= minLightValue) {
			setElevation(yPosition - yIncrement);
			break;
		}
	}
}
