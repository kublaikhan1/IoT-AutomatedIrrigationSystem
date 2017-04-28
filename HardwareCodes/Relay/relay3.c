#include <stdio.h>
#include <wiringPi.h>

#define RELAY1 21
#define RELAY2 22
#define RELAY3 23

int main(void)
{
	if(wiringPiSetup() == -1)
	{
		printf("wiringPi setup failed");
		return 1;
	}

	pinMode(RELAY1, OUTPUT);
	pinMode(RELAY2, OUTPUT);
	pinMode(RELAY3, OUTPUT);

	while(1)
	{
		digitalWrite(RELAY1, LOW);
		printf("RELAY 1 open\n");
		delay(250);
		digitalWrite(RELAY2, LOW);
		printf("RELAY 2 open\n");
		delay(250);
		digitalWrite(RELAY3, LOW);
		printf("RELAY 3 open\n");
		delay(500);
		digitalWrite(RELAY1, HIGH);
		printf("RELAY 1 closed\n");
		delay(250);
		digitalWrite(RELAY2, HIGH);
		printf("RELAY 2 closed\n");
		delay(250);
		digitalWrite(RELAY3, HIGH);
		printf("RELAY 3 closed\n");
		printf("-------------------------\n\n");
		delay(750);
	}
	return 0;
}
