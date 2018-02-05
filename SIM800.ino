char charReceived;

void restartModem() {
	digitalWrite(D1, HIGH);
	delay(500);
	digitalWrite(D1, LOW);
	delay(500);
	digitalWrite(D1, HIGH);

	// Wait n seconds for the modem to reset
	delayWithBlink(20, "Resetting Modem");
	Serial.println("");
}

void startUp() {
	printHeader("Check UART communication");
	sendCommand("AT\r\n", 3);

	// printHeader("Set cell modules baud rate");
	// sendCommand("AT+IPR=19200\r\n", 1);

	// printHeader("Check if SIM is readable");
	// sendCommand("AT+CPIN?\r\n", 3);

	// printHeader("Set SMS to text mode");
	// sendCommand("AT+CMGF=1\r\n", 3);
}

void connect() {
	printHeader("Shutdown modem before bringing it back up");
	sendCommand("AT+CIPSHUT\r\n", 1);

	printHeader("Check GPRS status");
	sendCommand("AT+CGATT?\r\n", 1);

	printHeader("Set modem mode");
	sendCommand("AT+CIPMUX=1\r\n", 1);

	printHeader("Set APN");
	sendCommand("AT+CSTT=\"hologram\"\r\n", 1);

	printHeader("Bring up wireless connection");
	sendCommand("AT+CIICR\r\n", 5);

	printHeader("Get local IP address");
	sendCommand("AT+CIFSR\r\n", 1);

	printHeader("Set APN");
	sendCommand("AT+SAPBR=3,1,\"APN\",\"hologram\"\r\n", 2);

	printHeader("Set Bearer Settings");
	sendCommand("AT+SAPBR=1,1\r\n", 2);

	printHeader("Initialize HTTP Service");
	sendCommand("AT+HTTPINIT\r\n", 5);

	printHeader("Set Bearer profile identifier");
	sendCommand("AT+HTTPPARA=\"CID\",1\r\n", 1);
}

String sendGetRequestToDweet() {
	printHeader("Set HTTP client URL");
	sendCommand("AT+HTTPPARA=\"URL\",\"http://dweet.io/get/latest/dweet/for/mr-roboto\"\r\n", 1);

	printHeader("HTTP method action GET");
	sendCommand("AT+HTTPACTION=0\r\n", 10);

	printHeader("Read response from server");
	return sendCommand("AT+HTTPREAD\r\n", 1);
	
	// printHeader("Terminate HTTP Service");
	// sendCommand("AT+HTTPTERM\r\n", 1);
}

String sendCommand(char *cmd, int wait) {
	SIM800Serial.write(cmd);
	return getResponse(wait);
}

String getResponse(int wait) {
	String response;
	char c;
    unsigned long timerStart = millis();
    unsigned long timeout = wait * 1000;

	while(millis() - timerStart < timeout) {
        if(SIM800Serial.available()){
        	c = SIM800Serial.read();
            response = response + c;
        }
        Serial.available(); // For whatever reason if this call isn't here we get a stack error on the WEMOS
    }

    Serial.println("Response:\r\n" + response);
    return response;
}

void printHeader(String message) {
	Serial.println("==========================================");
	Serial.println(message);
	Serial.println("==========================================");
}

void delayWithBlink(int waitSeconds, String message) {
	Serial.print(message);

	for(int i = 0; i < waitSeconds; i++) {
		blink(30, 1000);
		Serial.print(".");
	}
}
