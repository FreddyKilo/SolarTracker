void startTracking() {
	/*
		Get the values from the last dweet then use those values to help adjust the position
		of the solar panels to obtain max voltage
	*/
	getData();

	// Pick out all the valuable data we need from the server response
	parseResponse();

	// Set up the time variables to be able to format to a human readable time
	setupReadableTime();

	// If waking up from sunset, reset to neutral position.
	// If no morning postion, do an initial scan
	// Else just get the previous readings
	if (getJsonValue("sunset").equals("true")) {
		setNeutralPosition();
	} else {
		xPosition = getJsonValue("x").toInt();
		yPosition = getJsonValue("y").toInt();
	}

	// If the hour is greater than 6pm (about sunset) and less than 7am (about sunrise), stop
	// tracking and get the voltage reading periodically to monitor on freeboard
	if (hour < 5 || hour > 22) {
		setNeutralPosition();
		setData("time=" + getReadableTime() +
				"&sunset=true" +
				"&light=" +
				"&voltage=" + String(getVoltage()));
		Serial.println("Sleeping overnight...");
		if (hour > 6 && hour < 7) {
			// Wake up right at 7am
			int hoursTilMorning = 60 / minutes;
			ESP.deepSleep(hoursTilMorning * oneSecond)
		} else {
			// Sleep for an hour
			ESP.deepSleep(overnightSleepTime * oneSecond);
		}
	}

	initialScan();

	if (maxLightValue < minLightValue) {
		setNeutralPosition();
		// Wait for servos to stop or else we will get a battery reading with a significant voltage drop
		delay(2000);
	}

	/*
		Store current values for next position adjustment.
	*/
	setData("x=" + String(xPosition) +
			"&y=" + String(yPosition) +
			"&light=" + String(getLightValue()) +
			"&voltage=" + String(getVoltage()) +
			"&time=" + getReadableTime() +
			"&sunset=false");

	// Sleep until next scheduled adjustment
	Serial.println("Taking a nap...");
	ESP.deepSleep(sleepTime * oneSecond);
}

/*
	This will be called if there were no previous recorded servo positions.
*/
void initialScan() {
	
	setHomePosition();

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
