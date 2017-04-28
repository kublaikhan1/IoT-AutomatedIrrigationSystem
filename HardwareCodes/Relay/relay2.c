#include <stdio.h>
#include <wiringPi.h>

#define RELAY1 21
#define RELAY2 22

int main(void)
{
	if(wiringPiSetup() == -1)
	{
		printf("wiringPi setup failed");
		return 1;
	}

	pinMode(RELAY1, OUTPUT);
	pinMode(RELAY2, OUTPUT);

	while(1)
	{
		digitalWrite(RELAY1, LOW);
		printf("RELAY 1 open\n");
		delay(250);
		digitalWrite(RELAY2, LOW);
		printf("RELAY 2 open\n");
		delay(500);
		digitalWrite(RELAY1, HIGH);
		printf("RELAY 1 closed\n");
		delay(250);
		digitalWrite(RELAY2, HIGH);
		printf("RELAY 2 closed\n");
		printf("-------------------------\n\n");
		delay(500);
	}
	return 0;
}
