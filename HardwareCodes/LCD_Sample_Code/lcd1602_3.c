#include <stdio.h>
#include <stdlib.h>

#include <wiringPi.h>
#include <lcd.h>

#define RS 25
#define E  29
#define D4 28
#define D5 27
#define D6 26
#define D7 6

const unsigned char Buf[] = "S";

int main(void)
{
	int fd;
	int i;
	
	if(wiringPiSetup() == -1){
		exit(1);
	}
	
	printf("here\n");
	
	fd = lcdInit(2,16,4,RS,E,D4,D5,D6,D7,0,0,0,0);
	if (fd == -1){
		printf("lcdInit 1 failed\n") ;
		return 1;
	}
	sleep(1);
	
	printf("fd = %d\n", fd);
	
	lcdClear(fd);
	lcdPosition(fd, 15, 1); 
	lcdPuts(fd, "X");

	lcdPosition(fd, 0, 1); 
	lcdPuts(fd, Buf);

	sleep(1);
	lcdClear(fd);
	
/*	while(1){
		for(i=0;i<sizeof(Buf)-1;i++){
			lcdPosition(fd, i, 1);
			lcdPutchar(fd, *(Buf+i));
			delay(200);
		}
		//lcdPosition(fd, 0, 1); 
		lcdClear(fd);
		sleep(1);
	}
*/
	return 0;
}
