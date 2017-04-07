#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// A normal C function that is executed as a thread when its name
// is specified in pthread_create()
void funct();

void *soilMoisture(void *vargp)
{
     int f=0;
     while(f==0){
     sleep(10);   
     funct();}
     return NULL;
}

int main()
{
     pthread_t pt;
     int k=1;
     pthread_create(&pt, NULL, soilMoisture, NULL);
     while(k>0)
     {
       printf("This is the main thread (Measuring the water leverl) : %d\n", k);
       sleep(1);
     }
     return 0;
}

void funct()
{
  printf("******(Measuring soil moisture after sleeping for 10SEC)***********\n");
}
