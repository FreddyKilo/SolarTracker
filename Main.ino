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

	// If unable to connect to server, or if debug mode, set optimal position algorithmically
	if (!connected || debugMode) {
		setOptimalPostion();

	// Set the current position based on latitude, time of day, and day of year
	} else {
		setRelativePositionX(0);
		setRelativePositionY(0);

		// Store current values for freeboard.
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
	
	delay(daySleepTime * 1000L);
}
