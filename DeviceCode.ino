#include <Arduino.h>
#include <Wire.h>
#include "SparkFunHTU21D.h"
#include <BME280I2C.h>
#include <BH1750.h>

#define SERIAL_BAUD 115200
#define I2C_ADDR 0x38
#define IT_1_2 0x0 //1/2T
#define IT_1   0x1 //1T
#define IT_2   0x2 //2T
#define IT_4   0x3 //4T

void printBME280Data(Stream * client);

HTU21D myHumidity;
BH1750 lightMeter;
BME280I2C bme;

void setup()
{
	Serial.begin(SERIAL_BAUD);
	while (!Serial) {};
	Wire.begin();

	Wire.beginTransmission(I2C_ADDR);
	Wire.write((IT_1 << 2) | 0x02);
	Wire.endTransmission();

	myHumidity.begin();
	lightMeter.begin();
	bme.begin();
}

void loop()
{
	printBME280Data(&Serial);
	readHumidity();
	readLigth();
	readVu();

	Serial.println("---------------");
	delay(1000);
}



void readVu()
{
	byte msb = 0, lsb = 0;
	uint16_t uv;

	Wire.requestFrom(I2C_ADDR + 1, 1); //MSB
	delay(1);
	if (Wire.available())
		msb = Wire.read();

	Wire.requestFrom(I2C_ADDR + 0, 1); //LSB
	delay(1);
	if (Wire.available())
		lsb = Wire.read();

	uv = (msb << 8) | lsb;
	Serial.println(uv, DEC); //output in steps (16bit)
}

void readLigth()
{
	uint16_t lux = lightMeter.readLightLevel();
	Serial.print("Light: ");
	Serial.print(lux);
	Serial.println(" lx");
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
	float altitude = bme.alt(true);
	uint8_t pressureUnit(3);                                           // unit: B000 = Pa, B001 = hPa, B010 = Hg, B011 = atm, B100 = bar, B101 = torr, B110 = N/m^2, B111 = psi
	bme.read(pres, temp, hum, true, pressureUnit);                   // Parameters: (float& pressure, float& temp, float& humidity, bool celsius = false, uint8_t pressureUnit = 0x0)

	client->print("Temp: ");     client->print(temp); client->print("C"); client->println();
	client->print("Pressure: "); client->print(pres); client->print(" atm"); client->println();
	client->print("Altitude: "); client->print(altitude); client->print(("m")); client->println();
}