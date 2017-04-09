#include <Arduino.h>
#include <Wire.h>
#include "SparkFunHTU21D.h"
#include <BME280I2C.h>
#include <BH1750.h>
#include <Adafruit_ADS1015.h>

#define SERIAL_BAUD 115200
#define I2C_ADDR 0x38
#define pwmPin 0

int prevVal = LOW;
long th, tl, h, l, ppm;
unsigned long time;
void printBME280Data(Stream * client);

HTU21D myHumidity;
BH1750 lightMeter;
BME280I2C bme;
Adafruit_ADS1115 ads;

void setup()
{
	Serial.begin(SERIAL_BAUD);
	pinMode(pwmPin, INPUT);
	while (!Serial) {};
	Wire.begin();
	vuBegin();

	myHumidity.begin();
	lightMeter.begin();
	bme.begin();
	adsBegin();

	time = millis();
}

void loop()
{
	if (millis() > time + 2000) {
		printBME280Data(&Serial);
		readHumidity();
		readLigth();
		readVu();
		adsRead();

		Serial.println("CO2Sensor: PPM = " + String(ppm));
		Serial.println("---------------");
		time = millis();
	}

	readCo2();
}










void adsBegin() {
	// The ADC input range (or gain) can be changed via the following
	// functions, but be careful never to exceed VDD +0.3V max, or to
	// exceed the upper and lower limits if you adjust the input range!
	// Setting these values incorrectly may destroy your ADC!
	//                                                                ADS1015  ADS1115
	//                                                                -------  -------
	// ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
	// ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
	// ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
	// ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
	// ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
	// ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

	ads.begin();
}

void adsRead() {
	int16_t adc0, adc1, adc2, adc3;

	adc0 = ads.readADC_SingleEnded(0);
	Serial.print("VOC lavel: VOC = "); Serial.println(adc0);
}


void readCo2() {
	long tt = millis();
	int myVal = digitalRead(pwmPin);

	if (myVal == HIGH) {
		if (myVal != prevVal) {
			h = tt;
			tl = h - l;
			prevVal = myVal;
		}
	}
	else {
		if (myVal != prevVal) {
			l = tt;
			th = l - h;
			prevVal = myVal;
			ppm = 5000 * (th - 2) / (th + tl - 4);
		}
	}
}













void vuBegin() 
{
	Wire.beginTransmission(0x38);
	Wire.write((0x1 << 2) | 0x02);
	Wire.endTransmission();
}

void readVu()
{
	byte msb = 0, lsb = 0;
	uint16_t uv;

	Wire.requestFrom(0x38 + 1, 1); delay(1);
	if (Wire.available()) msb = Wire.read();

	Wire.requestFrom(0x38 + 0, 1); delay(1);
	if (Wire.available()) lsb = Wire.read();

	uv = (msb << 8) | lsb;
	Serial.print("Vumeter: "); Serial.println(uv, DEC);
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

	//client->print("Temp: ");     client->print(temp); client->print("C"); client->println();
	client->print("Pressure: "); client->print(pres); client->print(" atm"); client->println();
	client->print("Altitude: "); client->print(altitude); client->print(("m")); client->println();
}