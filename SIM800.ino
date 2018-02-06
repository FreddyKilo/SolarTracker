char charReceived;

void sim800Restart() {
	digitalWrite(D1, LOW);
	delay(500);
	digitalWrite(D1, HIGH);

	// Wait n seconds for the modem to reset
	delayWithBlink(20, "Resetting Modem");
	Serial.println("");
}

void sim800StartUp() {
	digitalWrite(D1, HIGH);

	printHeader("Check UART communication");
	sendCommand("AT\r\n", 1);
	
	printHeader("Check UART communication one more time");
	sendCommand("AT\r\n", 1);
	
	// printHeader("Set local timestamp");
	// sendCommand("AT+CLTS=1\r\n", 1);

	// printHeader("Save timestamp config");
	// sendCommand("AT&W\r\n", 1);

	// printHeader("Set cell modules baud rate");
	// sendCommand("AT+IPR=19200\r\n", 1);

	// printHeader("Check if SIM is readable");
	// sendCommand("AT+CPIN?\r\n", 3);

	// printHeader("Set SMS to text mode");
	// sendCommand("AT+CMGF=1\r\n", 3);
}

void sim800Connect() {
	printHeader("Shutdown modem before bringing it back up");
	sendCommand("AT+CIPSHUT\r\n", 1);

	// printHeader("Check GPRS status");
	// sendCommand("AT+CGATT?\r\n", 1);

	printHeader("Set modem mode");
	sendCommand("AT+CIPMUX=1\r\n", 1);

	printHeader("Set APN");
	sendCommand("AT+CSTT=\"hologram\"\r\n", 1);

	printHeader("Bring up wireless connection");
	sendCommand("AT+CIICR\r\n", 3);

	// printHeader("Get local IP address");
	// sendCommand("AT+CIFSR\r\n", 1);

	printHeader("Set APN");
	sendCommand("AT+SAPBR=3,1,\"APN\",\"hologram\"\r\n", 1);

	printHeader("Set Bearer Settings");
	sendCommand("AT+SAPBR=1,1\r\n", 1);

	printHeader("Initialize HTTP Service");
	sendCommand("AT+HTTPINIT\r\n", 3);

	printHeader("Set Bearer profile identifier");
	sendCommand("AT+HTTPPARA=\"CID\",1\r\n", 1);
}

String sim800PostToDweet() {
	printHeader("Set HTTP client URL");
	sendCommand("AT+HTTPPARA=\"URL\",\"http://dweet.io/dweet/for/mr-roboto\"\r\n", 1);

	printHeader("Set content type");
	sendCommand("AT+HTTPPARA=\"CONTENT\",\"application/json\"\r\n", 1);
	
	printHeader("Set data to send");
	sendCommand("AT+HTTPDATA=16,3000\r\n", 1);
	sendCommand("{\"hello\":\"thor\"}", 1);

	printHeader("HTTP method action POST");
	sendCommand("AT+HTTPACTION=1\r\n", 5);
}

void sim800TearDown() {
	printHeader("Terminate HTTP Service");
	sendCommand("AT+HTTPTERM\r\n", 1);

	printHeader("Set automatic sleep mode");
	sendCommand("AT+CSCLK=2\r\n", 1);
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

String sim800GetDateTime() {
	printHeader("Getting dateTime");
	String dateTime = sendCommand("AT+CCLK?\r\n", 1);
	dateTime = getSubstring(dateTime, '\r\n', 1);
	dateTime = getSubstring(dateTime, ' ', 1);
	dateTime = getSubstring(dateTime, '"', 1);
	Serial.println("dateTime: " + dateTime);
	return getSubstring(dateTime, '-', 0);
}
