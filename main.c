// Climate Control Software Client Code:

// Including libraries:
#include <stdio.h>
#include <stdlib.h>
#include "ccslib.h"

// Global Variables:
double desired;
double actual;
double level;
char sensorTempData[5];
char levelStr[15];
double temp = 0.0;
bool firstLoop = true;

#include <string.h>
#include <unistd.h>

// main function:
int main(int argc, char **argv) {

	extern_argc = argc;

	// Software Initialization:
	init();

	// TODO: config file:
	// Reading desired value from command line:
	desired = atof(argv[1]);

	// Infinite Loop:
	while (1) {
		// Read Sensor Data and transfer it to double:
		readData("/dev/temp_sensor", sensorTempData);
		actual = (double) atof(sensorTempData)/ 1000;

		// Sense an event in sensor:
		isEvent(temp, actual);
		temp = actual;

		// Calculate knob level and transfer it to string:
		level = PIDcontroller(desired, actual);
		sprintf(levelStr, "%f", level);

		// Write level to knob:
		writeData("/dev/temp_knob", levelStr);

		// Print to screen:
		printf("Sensor Reading = %.2f°C  ==>  Knob Level = %.0f\n",
				actual, level);
	}

	return SUCCESS;
}
