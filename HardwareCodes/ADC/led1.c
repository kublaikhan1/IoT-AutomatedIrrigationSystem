#include <stdio.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <bcm2835.h>

#define LED 18

int main(int ac, char *av[])
{
	/* Initialize the bcm2835 library */
	if(!bcm2835_init()) return 1;

	/*
	 * Sets direction of LED pin, which was defined
	 * earlier as RPI_GPIO_P1_12, to an output
	 */

	bcm2835_gpio_fsel(LED, BCM2835_GPIO_FSEL_OUTP);

	unsigned int delay = 1000;

	while(1)
	{
		bcm2835_gpio_set(LED); // Set turns pin to 1, HIGH, ON
		bcm2835_delay(delay);
		bcm2835_gpio_clr(LED); // Clr turns pin to 0, LOW, OFF
		bcm2835_delay(delay);
	}
}
