#include <Servo.h>

Servo servoX;
Servo servoY;

int xPosition;
int yPosition;

int sensorValue = 0;
int adjInterval = 15; // The time for next angle adjustment in minutes

void setup() {
  Serial.begin(9600);
  
  // Set servos to neurtal position
  xPosition = 90;
  yPosition = 90;
  servoX.attach(0);
  servoY.attach(1);
  servoX.write(xPosition);
  servoY.write(yPosition);
  
  // Get initial reading of sensor
  sensorValue = analogRead(A0);
  // Scan to find the highest light reading
  initialScan();
}

void initialScan(){
  // Move x-axis in the default direction while taking readings
  // Compare a few readings after movement
  // if the readings are decreasing, move in the other direction
  // if the readings are increasing, move until the reading reaches it's highest value
  // keep track of the servo position with the highest reading
  // once the reading starts to decrease after n readings move back to the highest position
}

void loop() {
  // Wait here for next interval (15 mins?)
  delay(adjInterval*60*1000)

  // See if we can find a higher reading in X (east/west)
  // Record the direction of movement from origin (pos or neg) and assume the next
  // movement should be in the same direction
  adjustAngleX();

  // Once we're good in X, let's find the highest reading for Y (north/south)
  // Record direction of movement for Y
  adjustAngleY();
  
  Serial.println(sensorValue);
  delay(200);
}

void adjustAngleX() {

}

void adjustAngleY() {
  
}

