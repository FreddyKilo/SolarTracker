
float getCurrent() {
	// switch to battery current input
	digitalWrite(D2, HIGH);
	delay(100);
	int sensorValue = analogRead(A0);

	// Calculate current with reading, somehow
	float current = 1.0;

	digitalWrite(D2, LOW);
	return current;
}

float getVoltage() {
	// switch to battery voltage input
	digitalWrite(D3, HIGH);
	delay(100);
	int sensorValue = analogRead(A0);

	// At 4.2v (max battery voltage) we get a reading of around 710
	float voltage = 4.2 * sensorValue / 713;

	digitalWrite(D3, LOW);
	return voltage;
}

int getLightValue() {
	// switch to ldr sensor
	digitalWrite(D4, HIGH);
	delay(100);
	int sensorValue = analogRead(A0);

	// In direct sunlight we get a reading of 800 and want our dashboard to have a max value of 100
	// int light = 100 * sensorValue / 800;

	digitalWrite(D4, LOW);
	return sensorValue;
}
