#!/usr/bin/python
 
import spidev
import time

#Define Variables
delay = 2
ldr_channel0 = 0
ldr_channel1 = 1

#Create SPI
spi = spidev.SpiDev()
spi.open(0, 0)
 
def readadc(adcnum):
    # read SPI data from the MCP3008, 8 channels in total
    if adcnum > 7 or adcnum < 0:
        return -1
    r = spi.xfer2([1, 8 + adcnum << 4, 0])
    data = ((r[1] & 3) << 8) + r[2]
    return data
    
 
while True:
    ldr_value0 = readadc(ldr_channel0)
    ldr_value1 = readadc(ldr_channel1)
    print "---------------------------------------"
    print("LDR 0 Value: %d" % ldr_value0)
    print("LDR 1 Value: %d" % ldr_value1)
    time.sleep(delay)
