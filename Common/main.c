#include <stdio.h>
#include <stdlib.h>
#include "ThingSpeakLinux.h"
#include <time.h>
#include <unistd.h>
/*
Number of arguments should be the same as the number of fields defined in the channel for ThingSpeak

Need to do Error Checking
*/

int main (int argc, char *argv[])
{
	time_t seed;
	time(&seed);
	srand(seed);
	
	/*
	ThisUpdates to  is a test main program from cmd Linux 
	Need to pass data sensor to function
	Sensor data is already stored in Array 
	*/

	int i,temp,water,humid,moist;
	float FieldsArray[100]={0};		// Data Sensor Array
	for(i=0;i<100;i++){
	  moist = rand()%20+1;
	  temp = rand()%60+1;
	  humid = rand()%70+1;
	  water = rand()%2; 
	  FieldsArray[i]=moist;
	  FieldsArray[i+1]=temp;
	  FieldsArray[i+2]=humid;
          FieldsArray[i+3]=water;	  
	  SendDataToThingSpeak(4, &FieldsArray[i],argv[1],sizeof(argv[1]));
	  fflush(stdout);
	  sleep(3);
          printf("[%f]\n",FieldsArray[i]);
	}
	
	
	//float FieldsArray[argc-1];
	//int i;
	//for(i=2;i<argc; i++)
	//	FieldsArray[i-2]=atof(argv[i]);
	
	//SendDataToThingSpeak(argc-2, &FieldsArray[0],argv[1],sizeof(argv[1]));
	
	

	return 0;
}
