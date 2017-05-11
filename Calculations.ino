
#define DEG_TO_RAD 0.01745329
#define PI 3.141592654
#define TWOPI 6.28318531

float Lon = -112.0740 * DEG_TO_RAD;
float Lat = 33.4484 * DEG_TO_RAD;
float T, JD_frac, L0, M, e, C, L_true, f, R, GrHrAngle, Obl, RA, Decl, HrAngle;
long JD_whole, JDx;
float azimuth;
float elev;

/* 
	This method calculates solar positions as a function of location, date, and time.
	The equations are from Jean Meeus, Astronomical Algorithms, Willmann-Bell, Inc., Richmond, VA
	(C) 2015, David Brooks, Institute for Earth Science Research and Education.
*/
void calculateSolarPosition() {
	JD_whole = JulianDate(year, month, day);
	JD_frac = (gmtHour + minutes / 60.0 / 3600.0) / 24.0 - 0.5;
	T = JD_whole - 2451545; T = (T + JD_frac) / 36525.;
	L0 = DEG_TO_RAD * floatmod(280.46645 + 36000.76983 * T, 360.0);
	M = DEG_TO_RAD * floatmod(357.5291 + 35999.0503 * T, 360.0);
	e = 0.016708617 - 0.000042037 * T;
	C = DEG_TO_RAD * ((1.9146 - 0.004847 * T) * sin(M) + (0.019993 - 0.000101 * T) * sin(2 * M) + 0.00029 * sin(3 * M));
	f = M + C;
	Obl = DEG_TO_RAD * (23 + 26 / 60.0 + 21.448 / 3600.0 - 46.815 / 3600 * T);
	JDx = JD_whole - 2451545;
	GrHrAngle = 280.46061837 + (360 * JDx) % 360 + 0.98564736629 * JDx + 360.98564736629 * JD_frac;
	GrHrAngle = floatmod(GrHrAngle, 360.0);
	L_true = floatmod(C + L0, TWOPI);
	R = 1.000001018 * (1 - e * e) / (1 + e * cos(f));
	RA = atan2(sin(L_true) * cos(Obl), cos(L_true));
	Decl = asin(sin(Obl) * sin(L_true));
	HrAngle = DEG_TO_RAD * GrHrAngle + Lon - RA;
	azimuth = PI + atan2(sin(HrAngle), cos(HrAngle) * sin(Lat) - tan(Decl) * cos(Lat));
	elev = asin(sin(Lat) * sin(Decl) + cos(Lat) * (cos(Decl) * cos(HrAngle)));
}

float getAzimuth() {
	return getAngleByRadians(azimuth);
}

float getElevation() {
	return getAngleByRadians(elev);
}

/*
	If using an esp8266, use this in lue of fmod() due to
	a bug in libm.a that causes a compile error
*/
double floatmod(double a, double b) {
    return (a - b * floor(a / b));
}

long JulianDate(int year, int month, int day) {
	long JD_whole;
	int A,B;
	if (month <= 2) {
		year--; month += 12;
	}
	A = year / 100; B = 2-A + A / 4;
	JD_whole = (long)(365.25 * (year + 4716)) + (int)(30.6001 * (month + 1)) + day + B-1524;
	return JD_whole;
}

float getLengthOfDay(float latitude, float earthTilt) {
	return 12 + asin(
			(sin(getRadians(latitude)) * sin(getRadians(earthTilt))) /
			(sin(getRadians(90 - earthTilt)) * cos(getRadians(latitude)))) / 
			getRadians(90) * 12;
}

float getRadians(float angle) {
	return angle * (PI / 180);
}

float getAngleByRadians(float radians) {
	return radians * (180 / PI);
}

/*
	Pass in a slope along with an input. Arduino map() will return
	an identical value, but what's the fun in that?
*/
int getLinearOutput(int x1, int x2, int y1, int y2, float input) {
	float slope = (y2 - y1) / (x2 - x1);
	float yIntercept = y1 - slope * x1;
	return slope * input + yIntercept;
}


void printCalculations() {
	Serial.println("");
	Serial.println("Year: " + String(year));
	Serial.println("Month: " + String(month));
	Serial.println("Day: " + String(day));
	Serial.println("Time: " + String(hour) + ":" + String(minutes));
	Serial.println("Elevation: " + String(getAngleByRadians(elev)));
	Serial.println("Azimuth: " + String(getAngleByRadians(azimuth)));
	Serial.println("");
}
