#include <stdio.h>
#include <stdlib.h>
#include <mcp3004.h>
#include <wiringPi.h>

#define BASE  100
#define RELAY1 21
#define RELAY2 22
#define RELAY3 23
#define RELAY4 24
#define FL_SWI 4
#define SPI_CH 0
#define PLANTS 8

void start();
int initWiring();
int waterLevel();
int readSoilMoist(int[]);

int main(void)
{
	int soilVals[PLANTS] = {0};
	int waterLvl;
	
	/* Initialize wiringPi library */
	if(wiringPiSetup() == -1)
	{
		printf("wiringPi setup failed");
		return 1;
	}

	
	
	printf("[HIGH ---- LOW]\n");
	waterLvl = waterLevel();
	printf("Water Level = %d\n\n", waterLvl);
	
	
	
	
	
	
	readSoilMoist(soilVals);
	
	if(soilVals[0] > 800 && waterLvl)
	{
		digitalWrite(RELAY1, LOW);
		printf("RELAY 1 open\n");
		delay(3000);
		digitalWrite(RELAY1, HIGH);
		printf("RELAY 1 closed\n");
	}
	
	
	if(!waterLvl)
	{
		printf("Water level is low!\n"); // send Tweet	
	}

	return 0;
}


/* 
 * Initialize Analog-to-Digital Converter
 * Initialize GPIO pin modes for each relay
 * Initialize GPIO pin for float switch
 */
void start()
{
	digitalWrite(FL_SWI, HIGH);
	mcp3004Setup(BASE, SPI_CH);
	pinMode(RELAY1, OUTPUT);
	pinMode(RELAY2, OUTPUT);
	pinMode(RELAY3, OUTPUT);
	pinMode(RELAY4, OUTPUT);
	pinMode(FL_SWI, OUTPUT);
}


/* Returns 0 if water level is low, 1 otherwise */
int waterLevel()
{
	return digitalRead(FL_SWI);
}


/* 
 * Reads channels 0-7 on MCP3008 chip over Serial 
 * Peripheral Interface (SPI).
 * Each analogue sensor returns a value from 0 - 1023
 * which is stored in the soilVals array
 * Value of 1023 is dry (i.e. air)
 * Value of ~400 is wet (i.e. pure water)
 */
int readSoilMoist(int soilVals[])
{
	int chan;
	
	for(chan = 0; chan < 8; ++chan)
	{
		soilVals[chan] = analogRead(BASE + chan);
		printf("Sensor %d:  %d\n", chan+1, soilVals[chan]);
	}
}



