
void startTracking() {
	
// ====================================================
// =================== NIGHT TIME =====================
// ====================================================

	// If the time is past sunset and before sunrise, stop
	// tracking and get the voltage reading periodically to monitor on freeboard
	if (!debug && connected && (getCurrentMinutes() < getSunrise() || getCurrentMinutes() > getSunset())) {
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
			return;
		}
		// Sleep for an hour
		ESP.deepSleep(nightSleepTime);
		return;
	}

// ====================================================
// =================== DAY TIME =======================
// ====================================================

	// Set to stationary position
	if (!connected || stationaryMode) {
		setStationaryPosition();
	
	// Set the current position based on lat, lon, time of day, and day of year
	} else {
		calculateSolarPosition();
		setAzimuth(getAzimuth(), 0);
		setElevation(getElevation(), 3000);
	}

	postFreeboardValues();
	Serial.println("Taking a nap...");
	ESP.deepSleep(daySleepTime - (millis() * 1030));
}
