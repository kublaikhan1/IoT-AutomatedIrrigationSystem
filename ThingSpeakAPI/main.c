#include <stdio.h>
#include <stdlib.h>
#include "ThingSpeakLinux.h"

/*
Number of arguments should be the same as the number of fields defined in the channel for ThingSpeak

Need to do Error Checking
*/

int main (int argc, char *argv[])
{
	/*
	This is a test main program from cmd Linux 
	Need to pass data sensor to function
	Sensor data is already stored in Array 
	*/

	float FieldsArray[argc-1];		// Data Sensor Array
	
	int i;
	for(i=2;i<argc; i++)
		FieldsArray[i-2]=atof(argv[i]);
	
	SendDataToThingSpeak(argc-2, &FieldsArray[0],argv[1],sizeof(argv[1]));
	
	return 0;
}
