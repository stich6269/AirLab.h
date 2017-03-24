#include <Wire.h>
#include "SparkFunHTU21D.h"


HTU21D myHumidity;

void setup()
{
	Serial.begin(9600);

	myHumidity.begin();
}

void loop()
{
	readHumidity();
}



void readHumidity() 
{
	float humd = myHumidity.readHumidity();
	float temp = myHumidity.readTemperature();

	Serial.print("Time:");
	Serial.print(millis());
	Serial.print(" Temperature:");
	Serial.print(temp, 1);
	Serial.print("C");
	Serial.print(" Humidity:");
	Serial.print(humd, 1);
	Serial.print("%");

	Serial.println();
	delay(1000);
}


