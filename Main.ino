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

	// If the time is past sunset and before sunrise, stop
	// tracking and get the voltage reading periodically to monitor on freeboard
	if (getCurrentMinutes() < getSunrise() || getCurrentMinutes() > getSunset()) {
		setNeutralPosition();
		setData("x=" + String(xPosition) +
				"&y=" + String(yPosition) +
				"&light=" +
				"&voltage=" + String(getVoltage()) +
				"&time=" + getReadableTime() +
				"&sunset=true");
		Serial.println("Sleeping overnight...");
		// If it's after midnight and there is less than an hour before sunrise
		if (getSunrise() - getCurrentMinutes() >= 0 && getSunrise() - getCurrentMinutes() < 60) {
			// Wake up a little after sunrise
			int secondsUntilSunrise = (getSunrise() + 3 - getCurrentMinutes()) * 60;
			ESP.deepSleep(secondsUntilSunrise * oneSecond);
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

	// Set the current position based on lat, lon, time of day, and day of year
	} else {
		calculateSolarPosition();
		setAzimuth(getAzimuth(), 0);
		setElevation(getElevation(), 3000);

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
