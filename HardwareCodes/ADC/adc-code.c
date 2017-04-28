#include <stdio.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <mcp3004.h>

#define BASE 100
#define SPI_CHANNEL 0

int main(int ac, char *av[])
{
	int chan, x;
	
	/* Initialize wiringPi library */
	if(wiringPiSetup() == -1)
	{
		printf("wiringPi setup failed");
		return 1;
	}
	
	
	/* Initiliaze mcp3004 library */
	/* 3004 and 3008 are the same 4/8 channels */
	mcp3004Setup(BASE, SPI_CHANNEL); // 
	
	
	for(chan = 0; chan < 8; ++chan)
	{
		x = analogRead(BASE + chan);
		printf("%d\n", x);
	}
	return 0;
}
