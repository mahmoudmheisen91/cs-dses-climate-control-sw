// Climate Control Software:

// Including Libraries:
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "ccslib.h"

// Global Variables:
double desired;
double actual;
int command;
char sensorTempData[5];
char commandStr[15];
double temp = 0.0;
bool firstLoop = true;

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
		// TODO: MESSY
		// TASK 1: The Local Controller:

		// Read Sensor Data:
		readData("/dev/temp_sensor", sensorTempData);
		actual = (double) atof(sensorTempData)/ 1000;

		// sense an event in sensor:
		isEvent(temp, actual);
		temp = actual;

		// command (set level) to knob:
		command = PIDcontroller(desired, actual);
		sprintf(commandStr, "%d", command);

		// Write data to knob:
		writeData("/dev/temp_knob", commandStr);

		// Print to screen:
		printf("Sensor Reading = %.2f°C  ==>  Knob Level = %s\n",
				actual, commandStr);
	}

	return SUCCESS;
}
