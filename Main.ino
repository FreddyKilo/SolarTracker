void go() {
	/*
		Get the values from the last dweet then use those values to help adjust the position
		of the solar panels to obtain max voltage
	*/
	getData();
	parseResponse();

	if (timeOfDay.equals("")) {
		ESP.deepSleep(oneSecond);
	}

	// Let's use the 12 hour clock system with AM and PM :)
	if (hour > 11) {
		timeSuffix = "p";
		if (hour > 12) {
			twelveHour = hour % 12;
		} else {
			twelveHour = hour;
		}

	} else if (hour == 0) {
		twelveHour = 12;
	} else {
		twelveHour = hour;
	}

	// If minutes are less than 10, we need to add a zero before the number i.e. 8 = 08
	String strMinutes;
	if (minutes < 10) {
		strMinutes = "0" + String(minutes);
	} else {
		strMinutes = String(minutes);
	}

	// If waking up from sunset, reset to morning position.
	// If no morning postion, do an initial scan
	// Else just get the previous readings
	if (getJsonValue("sunset").equals("true") && getJsonValue("morningX").length() > 0) {
		xPosition = getJsonValue("morningX").toInt();
		yPosition = getJsonValue("morningY").toInt();
	} else if (getJsonValue("sunset").equals("true")) {
		initialScan();
	} else {
		xPosition = getJsonValue("x").toInt();
		yPosition = getJsonValue("y").toInt();
		dir = getJsonValue("direction");
	}

	// If the hour is greater than 6pm (about sunset) and less than 7am (about sunrise), stop
	// tracking and get the voltage reading periodically to monitor on freeboard
	if (hour < 7 || hour > 18) {
		setData("time=" + String(twelveHour) + ":" + strMinutes + timeSuffix +
				"&sunset=true" +
				"&light=" +
				"&voltage=" + String(getVoltage()));
		Serial.println("See you in the morning...");
		ESP.deepSleep(overnightSleepTime * oneSecond);
	}

	/*
		Using previous values, make adjustments to get the most amount of sunlight possible
	*/

	// Get current light reading
	// If reading is less than last reading, we need to re-position. Use values from last reading
	// to adjust position. If the direction of the last reading was positive, check for a greater
	// reading in the positive direction. Keep track of light sensor value with coresponding servo
	// position. If there are more that 3 consecutive lesser readings, stop re-position and move to
	// servo position mapped to highest sensor value.

	//  servoX.attach(0);
	//  servoY.attach(1);
	//  servoX.write(xPosition);
	//  servoY.write(yPosition);

	adjustAngleX(3);
	adjustAngleY(1);

	/*
		Store current values for next position adjustment.
	*/

	setData("x=" + String(xPosition) +
			"&y=" + String(yPosition) +
			"&light=" + String(getLightValue()) +
			"&voltage=" + String(getVoltage()) +
			"&time=" + String(twelveHour) + ":" + strMinutes + timeSuffix +
			"&sunset=false");

	// Sleep until next scheduled adjustment
	Serial.println("Taking a nap...");
	ESP.deepSleep(sleepTime * oneSecond);
}