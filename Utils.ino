
String months[13] = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

/*
	Get the hour, minutes, and json String from a GET request
*/
void parseResponse() {
	unsigned long timeout = millis();

	while (client.available() == 0) {

		// If the client times out, sleep 5 mins and try again
		if (millis() - timeout > 5000) {
			Serial.println(">>> Client Timeout !");
			ESP.deepSleep(300 * oneSecond);
		}
	}

	while (client.available()) {
		String line = client.readStringUntil('\r');
		// Print each line from the server response
		Serial.println(line);

		// Get the time and convert the hour to Arizona time
		if (line.indexOf("Date:") > -1) {
			year = getSubstring(line, ' ', 4).toInt();
			month = getMonthNum(getSubstring(line, ' ', 3));
			day = getSubstring(line, ' ', 2).toInt();
			timeOfDay = getSubstring(line, ' ', 5);
			hour = (getSubstring(timeOfDay, ':', 0).toInt() + 17) % 24;
			gmtHour = getSubstring(timeOfDay, ':', 0).toInt();
			minutes = (getSubstring(timeOfDay, ':', 1).toInt());
		}

		// Get the content from response Json
		if (line.indexOf('{') > -1) {
			jsonContent = line.substring(line.indexOf("\"content\":{") + 11, line.indexOf("}"));
			date = line.substring(line.indexOf("\"created\":") + 10, line.indexOf(",\"content\""));
			// month = getSubstring(date, '-', 1).toInt();
		}
	}
}

void parseModemResponse(String response) {
	
}

void setupReadableTime() {
	// If we didn't get a time value from the response, reset and try again in 5 minuites. Obtaining current time is critical
	if (timeOfDay.equals("")) {
		ESP.deepSleep(oneSecond * 300);
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
	if (minutes < 10) {
		strMinutes = "0" + String(minutes);
	} else {
		strMinutes = String(minutes);
	}
}

/*
	Get a human readable 12-hour time format
*/
String getReadableTime() {
	return String(twelveHour) + ":" + strMinutes + timeSuffix;
}

/*
	Get a value from the content of the response Json
	@param key - The key associated with the value
	@returns - The value
*/
String getJsonValue(String key) {
	String value = "";

	for (int i = 0; i < 6; i++) {
		String subString = getSubstring(jsonContent, ',', i);

		if (subString.indexOf(key) == 1) {
			value = subString.substring(subString.indexOf(':') + 1);
		}
	}
	return value;
}

/*
	Got this from stack overflow https://goo.gl/HZIMaU
	Get a substring from a String.

	@param line - The original String
	@param separator - The char(s) at where to separate
	@param index - The index of the substring to return
	@returns - The substring at given index or empty String if not found
*/
String getSubstring(String line, char separator, int index) {
	int found = 0;
	int strIndex[] = { 0, -1 };
	int maxIndex = line.length() - 1;

	for (int i = 0; i <= maxIndex && found <= index; i++) {
		if (line.charAt(i) == separator || i == maxIndex) {
			found++;
			strIndex[0] = strIndex[1] + 1;
			strIndex[1] = (i == maxIndex) ? i + 1 : i;
		}
	}
	return (found > index ? line.substring(strIndex[0], strIndex[1]) : "");
}

int getMonthNum(String month) {
	for (int i = 0; i < sizeof(months); i++) {
		if (month == months[i]) {
			return i;
		}
	}
	return 0;
}

void blink(int length, int interval) {
	digitalWrite(LED_BUILTIN, LOW);
	delay(length);
	digitalWrite(LED_BUILTIN, HIGH);
	delay(interval - length);
}
