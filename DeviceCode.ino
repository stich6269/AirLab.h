#include <Wire.h>
#include "SparkFunHTU21D.h"
#include <BME280I2C.h>
#include <Wire.h>   

#define SERIAL_BAUD 115200
bool metric = true;

void printBME280Data(Stream * client);
void printBME280CalculatedData(Stream* client);

HTU21D myHumidity;
BME280I2C bme;

void setup()
{
	Serial.begin(SERIAL_BAUD);
	while (!Serial) {}

	myHumidity.begin();
	bme.begin();
}

void loop()
{
	printBME280Data(&Serial);
	readHumidity();
	delay(1000);
	Serial.println("---------------");
}



void readHumidity() 
{
	float humd = myHumidity.readHumidity();
	float temp = myHumidity.readTemperature();

	Serial.print("Temperature: "); Serial.print(temp, 1); Serial.print("C"); Serial.println();
	Serial.print ("Humidity: "); Serial.print(humd, 1); Serial.print("%"); Serial.println();
}


void printBME280Data(Stream* client) 
{
	float temp(NAN), hum(NAN), pres(NAN);
	float altitude = bme.alt(metric);
	uint8_t pressureUnit(3);                                           // unit: B000 = Pa, B001 = hPa, B010 = Hg, B011 = atm, B100 = bar, B101 = torr, B110 = N/m^2, B111 = psi
	bme.read(pres, temp, hum, metric, pressureUnit);                   // Parameters: (float& pressure, float& temp, float& humidity, bool celsius = false, uint8_t pressureUnit = 0x0)

	client->print("Temp: ");     client->print(temp); client->print("C"); client->println();
	client->print("Pressure: "); client->print(pres); client->print(" atm"); client->println();
	client->print("Altitude: "); client->print(altitude); client->print((metric ? "m" : "ft")); client->println();
}