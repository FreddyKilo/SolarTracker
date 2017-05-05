/* 
	This program calculates solar positions as a function of location, date, and time.
	The equations are from Jean Meeus, Astronomical Algorithms, Willmann-Bell, Inc., Richmond, VA
	(C) 2015, David Brooks, Institute for Earth Science Research and Education.
*/

#define DEG_TO_RAD 0.01745329
#define PI 3.141592654
#define TWOPI 6.28318531

int second = 0;
float Lon = -112.0740 * DEG_TO_RAD;
float Lat = 33.4484 * DEG_TO_RAD;
float T, JD_frac, L0, M, e, C, L_true, f, R, GrHrAngle, Obl, RA, Decl, HrAngle;
long JD_whole, JDx;

void calculateSolarPosition(int year, int month, int day, int hour, int minutes) {

	JD_whole = JulianDate(year, month, day);
	JD_frac = (hour + minutes / 60.0 + second / 3600.0) / 24.0 - 0.5;
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
	printCalculations();
}

float getAzimuth() {
	return getAngleByRadians(azimuth);
}

float getElevation() {
	return getAngleByRadians(elev);
}

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

void printCalculations() {
	Serial.println("");
	Serial.println("Year: " + String(year));
	Serial.println("Month: " + String(month));
	Serial.println("Day: " + String(day));
	Serial.println("Time: " + String(hour) + ":" + String(minutes));
	Serial.println("Elevation: " + String(elev / DEG_TO_RAD, 3));
	Serial.println("Azimuth: " + String(azimuth / DEG_TO_RAD, 3));
	Serial.println("");
}
