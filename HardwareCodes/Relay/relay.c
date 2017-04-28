#include <stdio.h>
#include <wiringPi.h>

#define RELAY 21

int main(void)
{
	if(wiringPiSetup() == -1)
	{
		printf("wiringPi setup failed");
		return 1;
	}

	pinMode(RELAY, OUTPUT);

	while(1)
	{
		digitalWrite(RELAY, LOW);
		printf("RELAY 1 open\n");
		delay(500);
		
		
		
		digitalWrite(RELAY, HIGH);
		printf("RELAY 1 closed\n");
		printf("-------------------------\n\n");
		delay(500);
	}
	return 0;
}
