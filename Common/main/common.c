#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <mcp3004.h>
#include <pthread.h>
#include <wiringPi.h>
#include "ThingSpeakLinux.h"


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
#define NUM_PLANTS 2

/* Definitions for DHT22 temp/hum sensor */
#define MAX_TIMINGS 85
#define DHT_PIN 5

void start();
int getWaterLevel();
void *soilThread();
void getSoilMoisture(int[],float[]);
double getWaterUsage(double);
void read_dht_data();
void temperatureReadings();
void activateRelays();

int data[5] = { 0, 0, 0, 0, 0 };
int soilVals[NUM_PLANTS] = {1000};
float soilPercent[NUM_PLANTS] = {100.0};
float humtemp[3] = {0.0};
float FieldsArrayPlant1[6]={0.0};
float FieldsArrayPlant2[6]={0.0};
int waterLvl;



int main(void)
{
	pthread_t pt;

	if(wiringPiSetup() == -1)
	{
		printf("wiringPi setup failed");
		return 1;
	}
	
	start();
	pthread_create(&pt, NULL, soilThread, NULL);
	
	int count = 1;
	
	while(count == 1)
	{		
		if(!waterLvl)
		{
			
			//printf("Exiting program!\n");
			//count++;
			//exit(1);
		}
		
		//activateRelays();
		sleep(5);
	}

	return 0;
}


/* ------------------------------------------------
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


/* ------------------------------------------------
 * Returns 0 if water level is low, 1 otherwise
 * -----------------------------------------------*/
 
int getWaterLevel()
{
	return digitalRead(FL_SWI);
}


/* ---------------------------------------------------------------
 * Reads channels 0-7 on MCP3008 chip over Serial 
 * Peripheral Interface (SPI).
 * Each analogue sensor returns a value from 0 ~ 600
 * Value of    0 is dry (i.e. air)
 * Value of ~600 is wet (i.e. pure water)
 * --------------------------------------------------------------*/
 
void getSoilMoisture(int soilVals[], float soilPercent[])
{
	int chan, i;
	int sum = 0;
	
	for(chan = 0; chan < NUM_PLANTS; ++chan)
	{
		
		for(i = 0; i < 2000; i++)
			sum += (1023 - analogRead(BASE + chan));
			
		soilVals[chan] = sum / 2000;
		soilPercent[chan] = (float) ((soilVals[chan] / 800.0) * 100.0);

		if(soilVals[chan] < 800)
			printf("PLANT #%d moisture: %3.2lf%\n", chan+1, soilPercent[chan]);
		
		sum = 0;	
	}
}


/* ------------------------------------------------
 * Returns the amount of water  
 * used by the pump in Liters
 * Pump waters at a rate of 300L/H
 * timeWatered is recorded in seconds
 * -----------------------------------------------*/
 
double getWaterUsage(double timeWatered)
{
	return 300 * timeWatered / 3600;
}


/* ------------------------------------------------
 * Thread function which will collect the soil data  
 * and temperature data
 * If the water level is LOW, then the thread will
 * die
 * -----------------------------------------------*/
 
void *soilThread()
{
	char apiKey1[] = "KQ0EYMR9E7QY7500";
	char apiKey2[] = "Z8725FTQG09C3FIK";
	
	while(1)
	{
		printf("Water Level = %d\n\n", waterLvl = getWaterLevel());
		if(!waterLvl)
		{
			printf("---  Water level is low  ---\n");
			printf("--- Pump will not activate ---\n\n");
		}
		getSoilMoisture(soilVals, soilPercent);
		temperatureReadings();
		
		activateRelays();
		
		int i;
		for(i = 0; i < 6; i++)
		{
			SendDataToThingSpeak(6, &FieldsArrayPlant1[i],apiKey1,sizeof(apiKey1));
			SendDataToThingSpeak(6, &FieldsArrayPlant2[i],apiKey2,sizeof(apiKey2));
		}
		
		printf("\n-------------------------------------------\n\n");
		
		sleep(3);
	}

	return NULL;
}


/* -----------------------------------------------------
 * Function to take 250 temperature / humidity  
 * readings based on the read_dht_data() function
 * The sensor's data is sometimes corrupt, which is the
 * result of a faulty sensor - even the read_dht_data()
 * function accounts for this... So 250 measurements
 * will ensure that at least one value is recorded
 * ----------------------------------------------------*/
 
void temperatureReadings()
{
	int count = 0;
	while(1)
	{
		read_dht_data();
		if( !( humtemp[0] == 0.0 || humtemp[1] == 0.0 || humtemp[2] == 0.0 ) || (humtemp[0] >= 100.0 || humtemp[1] >= 100.0) || count > 249)
		{
			break;
		}
		count++;
	}
}


/* ----------------------------------------------------
 * Function to turn on the relays, which are connected
 * to the solenoid vales and the water pump.
 * If the soil moisture percentage is lower than 50%
 * for a given plant, the relay/valve associated with
 * that plant will open, and the pump will turn on.
 *
 * Function also records amount of water used, and
 * length of time that plants were watered.
 * ---------------------------------------------------*/
 
void activateRelays()
{
	double timeWatered = 0.0; double amtWatered = 0.0;
	int relays[] = {RELAY1,RELAY2,RELAY3,RELAY4};
	int numPlantsWatered=0; int numRelays=4;
	time_t waterTimeStart, waterTimeEnd;
	int i;
	
	for(i = 0; i < NUM_PLANTS; i++)
	{
		if(soilPercent[i] < 50.0 && waterLvl)
		{
			time(&waterTimeStart);
			digitalWrite(relays[i], LOW);
			digitalWrite(relays[2], LOW);
			printf("PLANT valve #%d open\n", (i+1) );
			//printf("PUMP ON \n" );
			
			delay(5000);
			
			digitalWrite(relays[i], HIGH);
			digitalWrite(relays[2], HIGH);
			printf("PLANT valve #%d closed\n\n", (i+1) );
			//printf("PUMP OFF \n");
			
			time(&waterTimeEnd);
			timeWatered = difftime(waterTimeEnd, waterTimeStart);
			amtWatered += getWaterUsage(timeWatered);
			numPlantsWatered++;
		}
	}
	
	// Fill ThingSpeak Arrays
	FieldsArrayPlant1[0] = soilPercent[0];
	FieldsArrayPlant1[1] = humtemp[2];
	FieldsArrayPlant1[2] = humtemp[0];
	FieldsArrayPlant1[3] = timeWatered;
	FieldsArrayPlant1[4] = amtWatered;
	FieldsArrayPlant1[5] = waterLvl;
	
	FieldsArrayPlant2[0] = soilPercent[1];
	FieldsArrayPlant2[1] = humtemp[2];
	FieldsArrayPlant2[2] = humtemp[0];
	FieldsArrayPlant2[3] = timeWatered;
	FieldsArrayPlant2[4] = amtWatered;
	FieldsArrayPlant2[5] = waterLvl;
	
	if(waterLvl)
	{
		printf("Time watered: %.2lf seconds\n", timeWatered*numPlantsWatered);
		printf("Amount of water used: %.2lf Liters\n\n\n", amtWatered);
	}
	else
	{
		printf("Time watered: %.2lf seconds\n", timeWatered*numPlantsWatered);
		printf("Amount of water used: %.2lf Liters\n\n\n", amtWatered);
	}
}


/*--------------------------------------------------------------------------------------
 *			:::IMPORTANT NOTE:::
 * 
 * This code was taken from the following source in order to avoid
 * re-inventing the wheel. The sensor used is DIGITAL, which is why
 * the code to extract meaningful data is so long. Using an ANALOG
 * sensor would have avoided the need to borrow this source code
 *
 * www.uugear.com/portfolio/read-dht1122-temperature-humidity-sensor-from-raspberry-pi/
 *
 *-------------------------------------------------------------------------------------*/
 
void read_dht_data()
{
	uint8_t laststate	= HIGH;
	uint8_t counter		= 0;
	uint8_t j		= 0, i;
	float hum, cel, far;

	data[0] = data[1] = data[2] = data[3] = data[4] = 0;

	/* pull pin down for 18 milliseconds */
	pinMode( DHT_PIN, OUTPUT );
	digitalWrite( DHT_PIN, LOW );
	delay( 18 );

	/* prepare to read the pin */
	pinMode( DHT_PIN, INPUT );

	/* detect change and read data */
	for ( i = 0; i < MAX_TIMINGS; i++ )
	{
		counter = 0;
		while ( digitalRead( DHT_PIN ) == laststate )
		{
			counter++;
			delayMicroseconds( 1 );
			if ( counter == 255 )
			{
				break;
			}
		}
		laststate = digitalRead( DHT_PIN );

		if ( counter == 255 )
			break;

		/* ignore first 3 transitions */
		if ( (i >= 4) && (i % 2 == 0) )
		{
			/* shove each bit into the storage bytes */
			data[j / 8] <<= 1;
			if ( counter > 16 )
				data[j / 8] |= 1;
			j++;
		}
	}

	/*
	 * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
	 * print it out if data is good
	 */
	if ( (j >= 40) &&
	     (data[4] == ( (data[0] + data[1] + data[2] + data[3]) & 0xFF) ) )
	{
		float h = (float)((data[0] << 8) + data[1]) / 10;
		if ( h > 100 )
		{
			h = data[0];	// for DHT11
		}
		float c = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10;
		if ( c > 125 )
		{
			c = data[2];	// for DHT11
		}
		if ( data[2] & 0x80 )
		{
			c = -c;
		}
		float f = c * 1.8f + 32;
		printf( "\n\nHumidity = %.1f %% Temperature = %.1f *C (%.1f *F)\n\n", h, c, f );
		
		hum = h;
		cel = c;
		far = f;
	}
	humtemp[0] = hum;
	humtemp[1] = cel;
	humtemp[2] = far;
}