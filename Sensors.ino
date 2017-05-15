
float getCurrent() {
	// switch to battery current input
	digitalWrite(D2, HIGH);
	delay(1000);
	int sensorValue = analogRead(A0);

	// Calculate current with reading, somehow
	float current = 1.0;

	digitalWrite(D2, LOW);
	return current;
}

float getVoltage() {
	// switch to battery voltage input
	digitalWrite(D3, HIGH);
	delay(1000);
	int sensorValue = analogRead(A0);

	// At 4.2v (max battery voltage) we get a reading of around 710
	float voltage = 4.2 * sensorValue / 713;

	// Get the percentage of battery level
	int percentage = (voltage - 3.2) * 100;

	digitalWrite(D3, LOW);
	return percentage;
}

int getLightValue() {
	// switch to ldr sensor
	digitalWrite(D4, HIGH);
	delay(1000);
	int sensorValue = analogRead(A0);

	// Keep track of hightest reading
	if (sensorValue > maxLightValue) {
		maxLightValue = sensorValue;
	}

	digitalWrite(D4, LOW);
	return sensorValue;
}
