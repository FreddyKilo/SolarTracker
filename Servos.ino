
/*
	This will be called if there were no previous recorded servo positions.
*/
void initialScan() {
	// Move x-axis in the default direction while taking readings
	// Compare a few readings after movement
	// if the readings are decreasing, move in the other direction
	// if the readings are increasing, move until the reading reaches it's highest value
	// keep track of the servo position with the highest reading
	// once the reading starts to decrease after n readings move back to the highest position
}

void adjustAngleX(int degreeAmount) {
	xPosition = xPosition + degreeAmount;
	servoX.write(xPosition);
}

void adjustAngleY(int degreeAmount) {
	yPosition = yPosition + degreeAmount;
	servoY.write(yPosition);
}
