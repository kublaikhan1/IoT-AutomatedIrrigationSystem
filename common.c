#include <lcd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <mcp3004.h>
#include <wiringPi.h>

/* Definitions for MCP3004/8 chipset */
#define BASE 100
#define SPI_CH 0

/* Definitions for 4-chan relay GPIO pins */
#define RELAY1 21
#define RELAY2 22
#define RELAY3 23
#define RELAY4 24

/* Float Switch GPIO pin */
#define FL_SWI 4

/* Define number of plants */
#define NUM_PLANTS 8

/* Define LCD pins */
#define RS 25
#define EN 29
#define D4 28
#define D5 27
#define D6 26
#define D7 6

void start();
int getWaterLevel();
void getSoilMoisture(int[]);
double getWaterUsage(double);

int main(void)
{
	int waterLvl, fd, i;
	int numPlantsWatered=0; int numRelays=4;
	double timeWatered, amtWatered;
	time_t waterTimeStart, waterTimeEnd;
	const unsigned char buf[] = "Soil Moisture:";
	int soilVals[NUM_PLANTS] = {0};
	int relays[] = {RELAY1,RELAY2,RELAY3,RELAY4};
	
	/* Initialize wiringPi */
	if(wiringPiSetup() == -1)
	{
		printf("wiringPi setup failed");
		return 1;
	}	
	
	/* Initialize LCD screen */
	if( (fd=lcdInit(2,16,4,RS,EN,D4,D5,D6,D7,0,0,0,0)) == -1)
	{
		printf("LCD setup failed");
		return 1;
	}
	
	/*
	lcdClear(fd);
	lcdPosition(fd,0,0);
	lcdPuts(fd,buf);
	lcdPosition(fd,0,1);
	lcdClear(fd);
	*/
	
	start();
	
	waterLvl = getWaterLevel();
	printf("Water Level = %d\n\n", waterLvl);

	getSoilMoisture(soilVals);
	printf("\n");

	for(i = 0; i < numRelays; i++)
	{
		if(soilVals[i] < 150 && waterLvl)
		{
			time(&waterTimeStart);
			digitalWrite(relays[i], LOW);
			printf("RELAY %d open\n", (i+1) );
			delay(3000);
			
			digitalWrite(relays[i], HIGH);
			printf("RELAY %d closed\n\n", (i+1) );
			time(&waterTimeEnd);
			timeWatered = difftime(waterTimeEnd, waterTimeStart);
			amtWatered += getWaterUsage(timeWatered);
			numPlantsWatered++;
		}
	}
	
	
	printf("Time watered: %.2lf seconds\n", timeWatered*numPlantsWatered);
	printf("Amount of water used: %.2lf Liters\n", amtWatered);
	
	
	if(!waterLvl)
	{
		printf("Water level is low!\n"); // send Tweet	
	}

	return 0;
}


/* -------------------------------------------------
 * Initialize Analog-to-Digital Converter
 * Initialize GPIO pin modes for each relay
 * Initialize GPIO pin for float switch
 * -----------------------------------------------*/
 
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


/* -------------------------------------------------
 * Returns 0 if water level is low, 1 otherwise
 * -----------------------------------------------*/
 
int getWaterLevel()
{
	return digitalRead(FL_SWI);
}


/* --------------------------------------------------
 * Reads channels 0-7 on MCP3008 chip over Serial 
 * Peripheral Interface (SPI).
 * Each analogue sensor returns a value from 0 ~ 600
 * Value of    0 is dry (i.e. air)
 * Value of ~600 is wet (i.e. pure water)
 * ------------------------------------------------*/
void getSoilMoisture(int soilVals[])
{
	int chan;
	
	for(chan = 0; chan < NUM_PLANTS; ++chan)
	{
		soilVals[chan] = 1023 - analogRead(BASE + chan);
		printf("Sensor %d:  %d\n", chan+1, soilVals[chan]);
	}
}


/* -------------------------------------------------
 * Returns the amount of water  
 * used by the pump in Liters
 * Pump waters at a rate of 300L/H
 * timeWatered is recorded in seconds
 * -----------------------------------------------*/
 
double getWaterUsage(double timeWatered)
{
	return 300 * timeWatered / 3600;
}
