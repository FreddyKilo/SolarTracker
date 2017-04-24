void startTracking() {
	
	// Make a request to dweet.io for stored data
	getData();
	// Pick out all the valuable data we need from the server response
	parseResponse();
	// Set up the time variables to be able to format to a human readable time
	setupReadableTime();

// ====================================================
// =================== NIGHT TIME =====================
// ====================================================

	// If the time is past 6pm (about sunset) and before 7am (about sunrise), stop
	// tracking and get the voltage reading periodically to monitor on freeboard
	if (hour < 7 || hour > 18) {
		setNeutralPosition();
		setData("x=" + String(xPosition) +
				"&y=" + String(yPosition) +
				"&light=" +
				"&voltage=" + String(getVoltage()) +
				"&time=" + getReadableTime() +
				"&sunset=true");
		Serial.println("Sleeping overnight...");
		if (hour == 6) {
			// For whatever reason, deepSleep wakes up a bit earlier than expected.
			// This should guarantee waking a few minutes after morning wake time
			int secondsTilMorning = (63 - minutes) * 60;
			ESP.deepSleep(secondsTilMorning * oneSecond);
		}
		// Sleep for an hour
		ESP.deepSleep(nightSleepTime * oneSecond);
	}

// ====================================================
// =================== DAY TIME =======================
// ====================================================

	// Get current positions from dweet
	xPosition = getJsonValue("x").toInt();
	yPosition = getJsonValue("y").toInt();

	// If unable to get servo positions, or if waking up from night, or if debug mode, start scan from relative home position
	if (getJsonValue("x").equals("") || getJsonValue("y").equals("") || getJsonValue("sunset").equals("true") || debug == true) {
		setRelativePositionX(-relativeThreshold);
		setRelativePositionY(-relativeThreshold);
		setOptimalPostion();

	// // If current light reading is less than previous, set optimal position
	// } else if (getLightValue() < getJsonValue("light").toInt()) {
	// 	setOptimalPostion();

	// Set the current position based on the time of day
	} else {
		setRelativePositionX(0);
		setRelativePositionY(0);
	}

	// Store current values for next position adjustment.
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
