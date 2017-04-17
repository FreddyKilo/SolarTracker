
/*
	Channel 4
	Min: 1408 (90 deg)
	Max: 2000 (150 deg)
*/
void setServoY(int degrees) {
	int channel = 4;
	maestro.setSpeed(channel, 15);
	maestro.setAcceleration(channel, 3);
	int target = ((degrees - 90) * 10) + 1400;
	int quarterMicroSec = target * 4;
	maestro.setTarget(channel, quarterMicroSec);
}

/*
	Channel 5
	Min: 1168 (20 deg)
	Max: 1744 (160 deg)
*/
void setServoX(int degrees) {
	int channel = 5;
	maestro.setSpeed(channel, 5);
	maestro.setAcceleration(channel, 2);
	int target = ((degrees - 20) * 4) + 1180;
	int quarterMicroSec = target * 4;
	maestro.setTarget(channel, quarterMicroSec);
}
