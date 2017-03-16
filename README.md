## Concept Proposal

### High-level Concept:
Create an IoT indoor home irrigation system for potted plants using a Raspberry Pi and various hardware components. The system will feature two soil moisture sensors in two separate potted plants that will send real time analog data back to the Pi. Software on the Pi will monitor these sensor levels periodically; when levels fall below a certain threshold that constitutes “dry” soil, a signal will be sent to activate a peristaltic pump attached to the Pi. The pump will distribute water from a plastic reservoir to the plants that are “dry” (i.e. either one or both) until the moisture sensor input rises above the threshold, at which point the pump will deactivate. Sensor readings and water usage data will be viewable via a ThingSpeak interface as well as notifications from Twitter. Having two sensor/plant setups will hold as a proof of concept that multiple plants and sensors can be linked into this IoT system. 

## Operation and Interaction:
This project will stand out among other competitive irrigation systems by our ability to connect multiple separate potted plants. Other projects made watering systems for either one plant, or a single bed of many plants, so the challenge here is to maintain two or more individual plants. 

Using different plants requires the use of solenoid valves that can redirect the flow of water to the proper plant. The pump and the valves require 12VDC, which is more than the output of the RPi’s 5VDC, so a relay will be used to connect these devices with the Pi. When a valve needs to be opened and the pump turned on, the Pi will send 5VDC to the relay, which will enable the hardware on the other side of the relay. 

The soil moisture sensors record data in analog, which is not standard for the Pi, so an MCP3008 chip will be used, which takes up to eight separate analog inputs at a time, converting them to digital. Although we will only be proving the irrigation system works for two plants (i.e. two analog sensors), up to eight will be feasible using this chip.
One additional sensor will be placed in the water reservoir where the pump is located - a water level float switch. When the water level drops to a certain point, the sensor will trip, sending a signal to the Pi. We will link this with Twitter as well to send a notification to the user that the reservoir needs to be refilled.

## Required Hardware:
1. RPi - in this case, we’ll be using Elliot’s RPi2 and Hamad’s RPi3
2. 2x Analog Capacitive soil moisture sensors
3. 2x Solenoid valves 12VDC
4. 1x Peristaltic pump 12VDC
5. 1x MCP3008 chip for analog to digital conversion
6. 1x 5VDC 4-channel relay
7. 1x Water level sensor

# Contributors
@khawajahamad
@ElliotGiese
