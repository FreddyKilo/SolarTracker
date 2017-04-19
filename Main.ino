void startTracking() {
	/*
		Get the values from the last dweet then use those values to help adjust the position
		of the solar panels to obtain max current
	*/
	getData();

	// Pick out all the valuable data we need from the server response
	parseResponse();

	// Set up the time variables to be able to format to a human readable time
	setupReadableTime();

	// If the hour is greater than 6pm (about sunset) and less than 7am (about sunrise), stop
	// tracking and get the voltage reading periodically to monitor on freeboard
	if (hour < 7 || hour > 18) {
		setNeutralPosition();
		setData("time=" + getReadableTime() +
				"&sunset=true" +
				"&light=" +
				"&voltage=" + String(getVoltage()));
		Serial.println("Sleeping overnight...");
		if (hour == 6) {
			// Wake up right at 7am
			if (minutes == 0) minutes = 60;
			int hoursTilMorning = minutes / 60;
			ESP.deepSleep(hoursTilMorning * oneSecond);
		}
		// Sleep for an hour
		ESP.deepSleep(nightSleepTime * oneSecond);
	}

	// Get the previous servo positions and set new position. If they dont exist, start scan from home position
	if (getJsonValue("x").equals("") || getJsonValue("y").equals("") || getJsonValue("sunset").equals("true")) {
		setHomePosition();
		setOptimalPostion();
	} else {
		xPosition = getJsonValue("x").toInt();
		yPosition = getJsonValue("y").toInt();
		setOptimalPostion();
	}

	if (maxLightValue < minLightValue) {
		setNeutralPosition();
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
	ESP.deepSleep(daySleepTime * oneSecond);
}
