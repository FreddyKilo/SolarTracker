/*
	Send some data to dweet.io

	@param params - The URL parameters to include with the request. i.e. "position=72&value=1500"
	This data will be stored and can later be retrieved as Json using getData()
*/
void setData(String params) {
	connectToDweet();
	if (connected) {
		Serial.println("");
		Serial.println("Sending dweet");
		client.print(String("GET /dweet/for/mr-roboto?" + params) + " HTTP/1.1\r\n" +
								 "Host: " + "dweet.io" + "\r\n" +
								 "Connection: close\r\n\r\n");
	}
}

/*
	Make a call to dweet.io. Response is saved in <WiFiClient client> and is parsed 
	with parseResponse()
*/
void getData() {
	connectToDweet();
	if (connected) {
		Serial.println("");
		Serial.println("Getting dweet");
		client.print(String("GET /get/latest/dweet/for/mr-roboto") + " HTTP/1.1\r\n" +
								 "Host: " + "dweet.io" + "\r\n" +
								 "Connection: close\r\n\r\n");
	}
}

/*
	Connect to dweet.io, this is where we will store our data
*/
void connectToDweet() {
	if (!client.connect("dweet.io", 80)) {
		Serial.println("connection failed");
		connected = false;
		return;
	}
	connected = true;
}

void postFreeboardValues() {
	// Store current values for freeboard.
	Serial.println("");
	Serial.println("Posting to freeboard...");
	setData("x=" + String(xPosition) +
			"&y=" + String(yPosition) +
			"&light=" + String(getLightValue()) +
			"&voltage=" + String(getVoltage()) +
			"&time=" + getReadableTime() +
			"&sunset=false");
}
